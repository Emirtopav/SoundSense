//       _____                       _  _____                            ______
//       _____ _____ ____ ___
//      / ____|                     | |/ ____|                          | ____|/
//      ____|  __ \___ \__ \ 
//     | (___   ___  _   _ _ __   __| | (___   ___ _ __  ___  ___ ______| |__  |
//     (___ | |__) |__) | ) |
//      \___ \ / _ \| | | | '_ \ / _` |\___ \ / _ \ '_ \/ __|/ _ \______|  __|
//      \___ \|  ___/|__ < / /
//      ____) | (_) | |_| | | | | (_| |____) |  __/ | | \__ \  __/      | |____
//      ____) | |    ___) / /_
//     |_____/ \___/ \__,_|_| |_|\__,_|_____/ \___|_| |_|___/\___|
//     |______|_____/|_|   |____/____|
//      ______     ________           _   _______
//     |  _ \ \   / /  ____|         (_) |__   __|
//     | |_) \ \_/ /| |__   _ __ ___  _ _ __| | ___  _ __   __ ___   __
//     |  _ < \   / |  __| | '_ ` _ \| | '__| |/ _ \| '_ \ / _` \ \ / /
//     | |_) | | |  | |____| | | | | | | |  | | (_) | |_) | (_| |\ V /
//     |____/  |_|  |______|_| |_| |_|_|_|  |_|\___/| .__/ \__,_| \_/
//                                                  | |
//                                                  |_|
// Developed By Emir Topav (Emirtopav)
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>

#include "bitmap.h"
#include "userConfig.h"

#if defined(USE_SH1106)
#include <Adafruit_SH110X.h>
#define COLOR_WHITE SH110X_WHITE
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
#elif defined(USE_SSD1306)
#include <Adafruit_SSD1306.h>
#define COLOR_WHITE WHITE
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
#else
#error "Lütfen userConfig.h içinde bir ekran tipi seçin!"
#endif

Adafruit_NeoPixel led(NUM_LEDS, WS2812_PIN, NEO_GRB + NEO_KHZ800);

float band_values[NUM_BANDS] = {0};
float smoothed_values[NUM_BANDS] = {0};
float peak_values[NUM_BANDS] = {0};
bool connected = false;

int currentMode = 0;
bool screensaverEnabled = true;
bool showPeaks = true;
unsigned long lastActiveTime = 0;
int ssX = 0, ssY = 0, ssDx = 2, ssDy = 2;

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);

  led.begin();
  led.show();

#if defined(USE_SH1106)
  if (!display.begin(OLED_ADDRESS, true)) {
#elif defined(USE_SSD1306)
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
#endif
    Serial.println(F("Ekran başlatılamadı!"));
    for (;;)
      ;
  }

  display.clearDisplay();
  display.display();

  showBitmap(startLogo);
  delay(2000);
  showBitmap(waitLogo);

  ssX = random(0, SCREEN_WIDTH - 25);
  ssY = random(0, SCREEN_HEIGHT - 10);
}

void loop() {
  if (!connected) {
    if (Serial.available()) {
      connected = true;
      lastActiveTime = millis();
      display.clearDisplay();
      display.display();
    } else {
      delay(100);
      return;
    }
  }

  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');

    if (data.startsWith("mode,")) {
      currentMode = data.substring(5).toInt();
    } else if (data.startsWith("peaks,")) {
      showPeaks = (data.substring(6).toInt() == 1);
    } else if (data.startsWith("flash")) {
      flashScreen();
    } else if (data.startsWith("ss,")) {
      screensaverEnabled = (data.substring(3).toInt() == 1);
    } else if (data.indexOf(',') > 0) {
      parseData(data);
      smoothData();

      float totalVolume = 0;
      for (int i = 0; i < NUM_BANDS; i++)
        totalVolume += band_values[i];

      if (totalVolume > 5.0) {
        lastActiveTime = millis();
      }
      updateLED();
    }
  }

  if (screensaverEnabled && (millis() - lastActiveTime > SCREENSAVER_TIMEOUT)) {
    drawScreensaver();
  } else {
    drawVisualizer();
  }
}

void flashScreen() {
  display.invertDisplay(true);
  delay(30);
  display.invertDisplay(false);
}

void updateLED() {
  for (int i = 0; i < NUM_LEDS; i++)
    led.setPixelColor(i, led.Color(0, 0, 0));
  led.show();
}

void drawScreensaver() {
  display.clearDisplay();
  ssX += ssDx;
  ssY += ssDy;
  if (ssX <= 0 || ssX >= SCREEN_WIDTH - 25)
    ssDx = -ssDx;
  if (ssY <= 0 || ssY >= SCREEN_HEIGHT - 8)
    ssDy = -ssDy;
  display.setCursor(ssX, ssY);
  display.setTextSize(1);
  display.setTextColor(COLOR_WHITE);
  display.print("Zzz...");
  display.display();
  delay(20);
}

void drawVisualizer() {
  display.clearDisplay();
  int barWidth = SCREEN_WIDTH / NUM_BANDS;

  for (int b = 0; b < NUM_BANDS; b++) {
    int barHeight = constrain(smoothed_values[b], 0, SCREEN_HEIGHT);
    if (barHeight >= peak_values[b]) {
      peak_values[b] = barHeight;
    } else {
      peak_values[b] -= 0.6;
      if (peak_values[b] < 0)
        peak_values[b] = 0;
    }

    int peakY = SCREEN_HEIGHT - (int)peak_values[b] - 1;
    if (peakY < 0)
      peakY = 0;
    if (peakY >= SCREEN_HEIGHT)
      peakY = SCREEN_HEIGHT - 1;

    int xPos = 0;
    if (currentMode == 1) { // Center Out
      int center = NUM_BANDS / 2;
      int offset = b / 2;
      int xIndex = (b % 2 == 0) ? (center + offset) : (center - offset - 1);
      xPos = xIndex * barWidth;
    } else { // Normal (0) and Dots (2)
      xPos = b * barWidth;
    }

    if (currentMode == 0 || currentMode == 1) {
      display.fillRect(xPos, SCREEN_HEIGHT - barHeight, barWidth - 1, barHeight,
                       COLOR_WHITE);
      if (showPeaks && peak_values[b] > 0)
        display.drawFastHLine(xPos, peakY, barWidth - 1, COLOR_WHITE);
    } else if (currentMode == 2) { // Dots Only
      int dotY = SCREEN_HEIGHT - barHeight - 1;
      if (dotY < 0)
        dotY = 0;
      if (dotY >= SCREEN_HEIGHT)
        dotY = SCREEN_HEIGHT - 1;
      display.drawFastHLine(xPos, dotY, barWidth - 1, COLOR_WHITE);
    }
  }
  display.display();
}

void parseData(String data) {
  int index = 0;
  int start = 0;
  int end = data.indexOf(',');

  while (end != -1 && index < NUM_BANDS) {
    band_values[index] = data.substring(start, end).toFloat();
    start = end + 1;
    end = data.indexOf(',', start);
    index++;
  }

  if (index < NUM_BANDS) {
    band_values[index] = data.substring(start).toFloat();
  }
}

void smoothData() {
  for (int i = 0; i < NUM_BANDS; i++) {
    smoothed_values[i] = smoothed_values[i] * (1 - SMOOTHING_FACTOR) +
                         band_values[i] * SMOOTHING_FACTOR;
  }
}

void showBitmap(const unsigned char *bitmap) {
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_WHITE);
  display.display();
}
