#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>

void terminal_init(void);
void terminal_toggle(void);
uint8_t terminal_is_open(void);
void terminal_render(void);
void terminal_handle_keyboard(char c);
void terminal_get_rect(int32_t* x, int32_t* y, uint32_t* w, uint32_t* h);
void terminal_set_pos(int32_t x, int32_t y);

#endif
