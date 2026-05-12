#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c.h"

#define SSD1306_WIDTH  128
#define SSD1306_HEIGHT 64

typedef struct {
    uint8_t i2c_addr;
    i2c_port_t i2c_port;
    uint8_t buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
} ssd1306_t;

esp_err_t ssd1306_init(ssd1306_t *dev, i2c_port_t port, uint8_t addr);
void ssd1306_clear(ssd1306_t *dev);
void ssd1306_draw_pixel(ssd1306_t *dev, int x, int y, bool on);
void ssd1306_draw_bitmap(ssd1306_t *dev, const uint8_t *bitmap);
esp_err_t ssd1306_refresh(ssd1306_t *dev);

#endif
