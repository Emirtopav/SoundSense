#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QWindow>
#include <thread>
#include <vector>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

#include "AudioAnalyzer.hpp"
#include "Globals.hpp"
#include "Settings.hpp"
#include "Renderer.hpp"
#include "BackendContext.hpp"
#include "PluginManager.hpp"
#include "LivePreviewItem.hpp"
#include "PresetManager.hpp"
#include "GameSenseClient.hpp"

// Global state for background threads (defined in Globals.cpp)

struct NetworkTask {
    std::vector<int> bitmap;
    int value;
};
std::queue<NetworkTask> g_NetworkQueue;
std::mutex g_NetworkMutex;
std::condition_variable g_NetworkCV;

void NetworkThread() {
    try {
        GameSenseClient client;
        client.RegisterGame();
        client.RegisterEvent();
        client.BindEvent();

        auto lastHeartbeat = std::chrono::steady_clock::now();

    while (g_Running) {
        NetworkTask task;
        {
            std::unique_lock<std::mutex> lock(g_NetworkMutex);
            g_NetworkCV.wait_for(lock, std::chrono::milliseconds(1), [] { return !g_NetworkQueue.empty() || !g_Running; });
            
            if (!g_Running) break;
            if (g_NetworkQueue.empty()) {
                // Periodically send heartbeat
                auto now = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::seconds>(now - lastHeartbeat).count() >= 10) {
                    client.Heartbeat();
                    lastHeartbeat = now;
                }
                continue;
            }
            
            task = std::move(g_NetworkQueue.front());
            g_NetworkQueue.pop();
        }
        
        client.SendFrame(task.bitmap, task.value);
        
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastHeartbeat).count() >= 10) {
            client.Heartbeat();
            lastHeartbeat = now;
        }
        }
    } catch (...) {}
}

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")

void AudioThread() {
    try {
        g_PluginManager.LoadPlugins("plugins");

        AudioAnalyzer analyzer;
        int frameValue = 0;
        auto lastSerial = std::chrono::steady_clock::now();
        
        while (g_Running) {
            auto now = std::chrono::steady_clock::now();
            auto loopStart = now;

      // Apply LFO Modulations
      VisualizerSettings activeSettings;
      {
          std::lock_guard<std::recursive_mutex> lock(g_SettingsMutex);
          activeSettings = g_Settings;
      }

      // 1. Get FFT Data
      std::vector<float> bands = analyzer.Get64Bands(activeSettings);
      
      // Trigger Detection
      auto events = g_TriggerDetector.Detect(bands);
      {
          std::lock_guard<std::recursive_mutex> lock(g_PluginMutex);
          for (const auto& ev : events) {
              for (auto& inst : g_PluginManager.loadedPlugins) {
                  if (inst.enabled && inst.plugin) {
                      inst.plugin->OnTrigger(ev);
                  }
              }
          }
      }

      // 2. Render Bitmap
      std::vector<int> bitmap = RenderBitmap(bands, activeSettings);
      
      auto renderStart = std::chrono::steady_clock::now();
      {
          std::lock_guard<std::recursive_mutex> lock(g_PluginMutex);
          if (!g_PluginManager.DoesAnyPluginOverrideBase()) {
              g_Scene.RenderAll(bands, bitmap);
          }
          g_PluginManager.RunPlugins(bands, bitmap);
      }

      // 3. Update UI/Global State
      {
        std::lock_guard<std::mutex> lock(g_BitmapMutex);
        g_CurrentBitmap = bitmap;
      }

      // 4. Send Data to ESP32 (if connected)
      if (g_Serial.IsConnected()) {
          static auto lastSerial = std::chrono::steady_clock::now();
          if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSerial).count() >= 16) {
              uint8_t serialData[65];
              serialData[0] = 0xFF; // Sync Header
              for (int i = 0; i < 64; i++) {
                  // Normalize float (0.0 - 1.0+) to uint8 (0 - 255)
                  int val = (int)(bands[i] * 255.0f);
                  if (val > 255) val = 255;
                  if (val < 0) val = 0;
                  serialData[i + 1] = (uint8_t)val;
              }
              g_Serial.Write(serialData, 65);
              lastSerial = now;
          }
      }

      // Queue network task at ~60 FPS for ultra-low latency
      static auto lastSSE = std::chrono::steady_clock::now();
      if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSSE).count() >= 16) {
          {
              std::lock_guard<std::mutex> lock(g_NetworkMutex);
              g_NetworkQueue.push({bitmap, frameValue++});
          }
          g_NetworkCV.notify_one();
          lastSSE = now;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
  } catch (...) {}
}

int main(int argc, char *argv[]) {
    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) {
      QFile logFile("qml_errors.log");
      if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ") << msg << "\n";
      }
    });
    
    qputenv("QT_QUICK_CONTROLS_STYLE", "Basic");
    
    QApplication app(argc, argv);
    
    qmlRegisterType<LivePreviewItem>("SoundSense", 1, 0, "LivePreviewItem");
    qmlRegisterUncreatableType<SceneModel>("SoundSense", 1, 0, "SceneModel", "Created by backend");
    qmlRegisterUncreatableType<PluginModel>("SoundSense", 1, 0, "PluginModel", "Created by backend");
    qmlRegisterUncreatableType<PresetModel>("SoundSense", 1, 0, "PresetModel", "Created by backend");

    g_PresetManager = new PresetManager();
    
    QQmlApplicationEngine engine;
    
    BackendContext backend;
    engine.rootContext()->setContextProperty("backend", &backend);

    // Load last active scene on startup
    g_Scene.LoadFromFile("active_scene.json");

    const QUrl url(QStringLiteral("qrc:/Main.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                       if (url == objUrl) {
                           if (!obj) {
                               QCoreApplication::exit(-1);
                           }
                       }
                     }, Qt::QueuedConnection);
                     
    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    QObject *rootObject = engine.rootObjects().first();
    QWindow *window = qobject_cast<QWindow*>(rootObject);

    // Setup System Tray
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(QIcon(":/logo.png"), &app);
    QMenu *trayMenu = new QMenu();
    
    QAction *showAction = new QAction("Show Dashboard", trayMenu);
    QAction *quitAction = new QAction("Exit", trayMenu);
    
    trayMenu->addAction(showAction);
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction);
    
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();

    QObject::connect(showAction, &QAction::triggered, [window]() {
        if (window) {
            window->show();
            window->raise();
            window->requestActivate();
        }
    });

    QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);

    QObject::connect(trayIcon, &QSystemTrayIcon::activated, [window](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
            if (window) {
                if (window->isVisible()) window->hide();
                else {
                    window->show();
                    window->raise();
                    window->requestActivate();
                }
            }
        }
    });

    if (window) {
        window->setProperty("visible", true);
    }

    // Delay background threads to ensure UI is stable
    QTimer::singleShot(2000, []() {
        try {
            std::thread audio(AudioThread);
            std::thread network(NetworkThread);
            audio.detach();
            network.detach();
        } catch (...) {}
    });
    
    int result = app.exec();
    g_Running = false;
    g_NetworkCV.notify_all();
    
    return result;
}
