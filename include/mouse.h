#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

void init_mouse(void);
void mouse_get_state(int32_t *x, int32_t *y, uint8_t *buttons);

#endif
