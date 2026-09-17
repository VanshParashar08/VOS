#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

void graphics_init(uint32_t fb_addr, uint32_t width, uint32_t height, uint32_t pitch, uint8_t bpp);
void put_pixel(uint32_t x, uint32_t y, uint32_t color);
void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void draw_char(uint32_t x, uint32_t y, char c, uint32_t fg_color, uint32_t bg_color);
void draw_string(uint32_t x, uint32_t y, const char* str, uint32_t fg_color, uint32_t bg_color);
void draw_char_scaled(uint32_t x, uint32_t y, char c, uint32_t fg_color, uint32_t bg_color, uint32_t scale);
void draw_string_scaled(uint32_t x, uint32_t y, const char* str, uint32_t fg_color, uint32_t bg_color, uint32_t scale);
void draw_image_scaled(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const uint32_t* data, uint32_t scale);
uint32_t get_screen_width(void);
uint32_t get_screen_height(void);

#endif
