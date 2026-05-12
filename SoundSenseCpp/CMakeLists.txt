cmake_minimum_required(VERSION 3.16)

project(SoundSenseCpp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt 6 components
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Widgets)

# Add our Qt Resource File
set(RESOURCE_FILES
    qml.qrc
)

# Application source files
set(SOURCES
    src/ui/main.cpp
    src/ui/BackendContext.cpp
    src/ui/PluginModel.cpp
    src/ui/PresetModel.cpp
    src/ui/SceneModel.cpp
    src/ui/LivePreviewItem.cpp
    
    src/core/AudioAnalyzer.cpp
    src/core/Globals.cpp
    src/core/Settings.cpp
    src/core/Renderer.cpp
    src/core/PresetManager.cpp
    src/core/TriggerSystem.cpp
    src/core/logo.cpp
    
    src/network/GameSenseClient.cpp
    src/hardware/SerialPort.cpp
    
    src/plugins/PluginManager.cpp
    src/plugins/Scene.cpp
    src/plugins/JsonPlugin.cpp
)

include_directories(
    src/core
    src/core/widgets
    src/ui
    src/network
    src/hardware
    src/plugins
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# If Windows, make it a GUI app (no console window pop-up)
if (WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${RESOURCE_FILES})
else()
    add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCE_FILES})
endif()

# Link the Qt 6 libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Widgets
)

# Windows specific libraries (for audio and other system calls)
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        comctl32
        shell32
        advapi32
        winhttp
    )
endif()
