#ifndef BGA_H
#define BGA_H

#include <stdint.h>

void bga_init(uint32_t width, uint32_t height, uint32_t bpp);
uint32_t bga_get_lfb(void);

#endif
