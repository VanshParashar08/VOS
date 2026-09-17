#include "../include/mouse.h"
#include "../include/isr.h"
#include "../include/ports.h"
#include "../include/graphics.h" // For bounds checking

static uint8_t mouse_cycle = 0;
static int8_t mouse_byte[3];
static int32_t mouse_x = 400; // Center of 800x600 screen
static int32_t mouse_y = 300;
static uint8_t mouse_buttons = 0;

static void mouse_wait(uint8_t a_type) {
    uint32_t timeout = 100000;
    if (a_type == 0) {
        while (timeout--) {
            if ((inb(0x64) & 1) == 1) return;
        }
    } else {
        while (timeout--) {
            if ((inb(0x64) & 2) == 0) return;
        }
    }
}

static void mouse_write(uint8_t a_write) {
    mouse_wait(1);
    outb(0x64, 0xD4);
    mouse_wait(1);
    outb(0x60, a_write);
}

static uint8_t mouse_read() {
    mouse_wait(0);
    return inb(0x60);
}

static void mouse_callback(registers_t *regs) {
    (void)regs;
    uint8_t status = inb(0x64);
    if (!(status & 0x20)) {
        return; // Not a mouse interrupt
    }
    
    switch (mouse_cycle) {
        case 0:
            mouse_byte[0] = inb(0x60);
            if (mouse_byte[0] & 0x08) { // Valid packet
                mouse_cycle++;
            }
            break;
        case 1:
            mouse_byte[1] = inb(0x60);
            mouse_cycle++;
            break;
        case 2:
            mouse_byte[2] = inb(0x60);
            mouse_cycle = 0;
            
            // Handle packet
            int x_movement = mouse_byte[1];
            int y_movement = mouse_byte[2];
            
            // Sign extension
            if (mouse_byte[0] & 0x10) x_movement |= 0xFFFFFF00;
            if (mouse_byte[0] & 0x20) y_movement |= 0xFFFFFF00;
            
            mouse_x += x_movement;
            mouse_y -= y_movement; // Mouse Y is inverted
            mouse_buttons = mouse_byte[0] & 0x07;
            
            // Bounds checking
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_x >= (int32_t)get_screen_width()) mouse_x = get_screen_width() - 1;
            if (mouse_y >= (int32_t)get_screen_height()) mouse_y = get_screen_height() - 1;
            
            break;
    }
}

void init_mouse(void) {
    uint8_t status;
    
    // Enable auxiliary mouse device
    mouse_wait(1);
    outb(0x64, 0xA8);
    
    // Enable interrupts
    mouse_wait(1);
    outb(0x64, 0x20);
    mouse_wait(0);
    status = (inb(0x60) | 2);
    mouse_wait(1);
    outb(0x64, 0x60);
    mouse_wait(1);
    outb(0x60, status);
    
    // Tell mouse to use default settings
    mouse_write(0xF6);
    mouse_read(); // ACK
    
    // Enable mouse
    mouse_write(0xF4);
    mouse_read(); // ACK
    
    register_interrupt_handler(44, mouse_callback);
}

void mouse_get_state(int32_t *x, int32_t *y, uint8_t *buttons) {
    *x = mouse_x;
    *y = mouse_y;
    *buttons = mouse_buttons;
}
