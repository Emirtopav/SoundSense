
/*      _______.  ______    __    __  .__   __.  _______      _______ .______
 * ___      .___  ___.  _______  */
/*     /       | /  __  \  |  |  |  | |  \ |  | |       \    |   ____||   _ \ /
 * \     |   \/   | |   ____| */
/*    |   (----`|  |  |  | |  |  |  | |   \|  | |  .--.  |   |  |__   |  |_)  |
 * /  ^  \    |  \  /  | |  |__    */
/*     \   \    |  |  |  | |  |  |  | |  . `  | |  |  |  |   |   __|  |      /
 * /  /_\  \   |  |\/|  | |   __|   */
/* .----)   |   |  `--'  | |  `--'  | |  |\   | |  '--'  |   |  |     |
 * |\  \----./  _____  \  |  |  |  | |  |____  */
/* |_______/     \______/   \______/  |__| \__| |_______/    |__|     | _|
 * `._____/__/     \__\ |__|  |__| |_______| */
/*                                                                                                                   */
/*  __    __       _______. _______ .______                    ______   ______
 * .__   __.  _______  __    _______   */
/* |  |  |  |     /       ||   ____||   _  \                  /      | /  __
 * \  |  \ |  | |   ____||  |  /  _____|  */
/* |  |  |  |    |   (----`|  |__   |  |_)  |                |  ,----'|  |  |  |
 * |   \|  | |  |__   |  | |  |  __    */
/* |  |  |  |     \   \    |   __|  |      /                 |  |     |  |  |  |
 * |  . `  | |   __|  |  | |  | |_ |   */
/* |  `--'  | .----)   |   |  |____ |  |\  \----.            |  `----.|  `--'  |
 * |  |\   | |  |     |  | |  |__| |   */
/*  \______/  |_______/    |_______|| _| `._____|             \______| \______/
 * |__| \__| |__|     |__|  \______|   */
#ifndef USER_CONFIG_H
#define USER_CONFIG_H

// Kullanılan OLED ekran tipi seçimi:
// SH1106 ekranı kullanmak için aşağıdaki satırı aktif hale getirin.
// #define USE_SH1106
// Eğer SSD1306 kullanıyorsanız yukarıdaki satırı yorum satırına alıp,
// aşağıdakini aktif edin.
#define USE_SSD1306

// I2C pin tanımlamaları:
// ESP32-S3 kartı için SDA (veri) pini GPIO numarası
#define I2C_SDA 8
// ESP32-S3 kartı için SCL (saat) pini GPIO numarası
#define I2C_SCL 9

// OLED ekranın I2C adresi:
// Genellikle 0x3C olur, ekranınıza göre değişebilir.
#define OLED_ADDRESS 0x3D

// OLED ekranın çözünürlüğü:
// Genişlik (piksel cinsinden)
#define SCREEN_WIDTH 128
// Yükseklik (piksel cinsinden)
#define SCREEN_HEIGHT 64

// Görselleştirme için kullanılacak frekans bant sayısı
#define NUM_BANDS 64

// Görsel yumuşatma katsayısı (0.0 - 1.0 arası bir değer):
#define SMOOTHING_FACTOR 0.6

// ----------------------------------------------------
// YENİ ÖZELLİKLER (Advanced Features)
// ----------------------------------------------------

// WS2812B RGB LED Ayarları
#define WS2812_PIN 48
#define NUM_LEDS 1

// Ekran Koruyucu (Screensaver) Ayarı
// Müzik çalmadığında (sessizlikte) kaç milisaniye sonra uyku moduna geçilsin?
#define SCREENSAVER_TIMEOUT 5000

#endif // USER_CONFIG_H
