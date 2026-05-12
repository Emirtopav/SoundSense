#include "ssd1306.h"
#include <string.h>

#define SSD1306_CMD_STREAM  0x00
#define SSD1306_DATA_STREAM 0x40

esp_err_t ssd1306_write_cmd(ssd1306_t *dev, uint8_t cmd) {
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (dev->i2c_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd_handle, 0x00, true);
    i2c_master_write_byte(cmd_handle, cmd, true);
    i2c_master_stop(cmd_handle);
    esp_err_t ret = i2c_master_cmd_begin(dev->i2c_port, cmd_handle, 100 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd_handle);
    return ret;
}

esp_err_t ssd1306_init(ssd1306_t *dev, i2c_port_t port, uint8_t addr) {
    dev->i2c_port = port;
    dev->i2c_addr = addr;
    memset(dev->buffer, 0, sizeof(dev->buffer));

    ssd1306_write_cmd(dev, 0xAE); // Display Off
    ssd1306_write_cmd(dev, 0xD5); // Set Display Clock Divide Ratio
    ssd1306_write_cmd(dev, 0x80);
    ssd1306_write_cmd(dev, 0xA8); // Set Multiplex Ratio
    ssd1306_write_cmd(dev, 0x3F);
    ssd1306_write_cmd(dev, 0xD3); // Set Display Offset
    ssd1306_write_cmd(dev, 0x00);
    ssd1306_write_cmd(dev, 0x40); // Set Display Start Line
    ssd1306_write_cmd(dev, 0x8D); // Charge Pump
    ssd1306_write_cmd(dev, 0x14);
    ssd1306_write_cmd(dev, 0x20); // Memory Addressing Mode
    ssd1306_write_cmd(dev, 0x00); // Horizontal
    ssd1306_write_cmd(dev, 0xA1); // Set Segment Re-map
    ssd1306_write_cmd(dev, 0xC8); // Set COM Output Scan Direction
    ssd1306_write_cmd(dev, 0xDA); // Set COM Pins Hardware Configuration
    ssd1306_write_cmd(dev, 0x12);
    ssd1306_write_cmd(dev, 0x81); // Set Contrast Control
    ssd1306_write_cmd(dev, 0xCF);
    ssd1306_write_cmd(dev, 0xD9); // Set Pre-charge Period
    ssd1306_write_cmd(dev, 0xF1);
    ssd1306_write_cmd(dev, 0xDB); // Set VCOMH Deselect Level
    ssd1306_write_cmd(dev, 0x40);
    ssd1306_write_cmd(dev, 0xA4); // Entire Display On
    ssd1306_write_cmd(dev, 0xA6); // Set Normal Display
    ssd1306_write_cmd(dev, 0xAF); // Display On
    return ESP_OK;
}

void ssd1306_clear(ssd1306_t *dev) {
    memset(dev->buffer, 0, sizeof(dev->buffer));
}

void ssd1306_draw_pixel(ssd1306_t *dev, int x, int y, bool on) {
    if (x < 0 || x >= SSD1306_WIDTH || y < 0 || y >= SSD1306_HEIGHT) return;
    if (on) dev->buffer[x + (y / 8) * SSD1306_WIDTH] |= (1 << (y % 8));
    else dev->buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
}

void ssd1306_draw_bitmap(ssd1306_t *dev, const uint8_t *bitmap) {
    memcpy(dev->buffer, bitmap, sizeof(dev->buffer));
}

esp_err_t ssd1306_refresh(ssd1306_t *dev) {
    ssd1306_write_cmd(dev, 0x21); // Column Address
    ssd1306_write_cmd(dev, 0x00);
    ssd1306_write_cmd(dev, 127);
    ssd1306_write_cmd(dev, 0x22); // Page Address
    ssd1306_write_cmd(dev, 0x00);
    ssd1306_write_cmd(dev, 7);

    for (int i = 0; i < 8; i++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (dev->i2c_addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, 0x40, true);
        i2c_master_write(cmd, &dev->buffer[i * SSD1306_WIDTH], SSD1306_WIDTH, true);
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(dev->i2c_port, cmd, 100 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);
    }
    return ESP_OK;
}
