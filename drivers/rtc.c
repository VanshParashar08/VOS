#include "../include/rtc.h"

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static int get_update_in_progress_flag() {
    outb(0x70, 0x0A);
    return (inb(0x71) & 0x80);
}

static uint8_t get_rtc_register(int reg) {
    outb(0x70, reg);
    return inb(0x71);
}

void rtc_init(void) {
    // Not strictly needed for basic reading
}

void rtc_read_time(uint8_t* h, uint8_t* m, uint8_t* s) {
    uint8_t last_s, last_m, last_h;
    uint8_t registerB;
    
    while (get_update_in_progress_flag());
    *s = get_rtc_register(0x00);
    *m = get_rtc_register(0x02);
    *h = get_rtc_register(0x04);
    
    do {
        last_s = *s;
        last_m = *m;
        last_h = *h;
        
        while (get_update_in_progress_flag());
        *s = get_rtc_register(0x00);
        *m = get_rtc_register(0x02);
        *h = get_rtc_register(0x04);
    } while ((last_s != *s) || (last_m != *m) || (last_h != *h));
    
    registerB = get_rtc_register(0x0B);
    
    if (!(registerB & 0x04)) {
        *s = (*s & 0x0F) + ((*s / 16) * 10);
        *m = (*m & 0x0F) + ((*m / 16) * 10);
        *h = ( (*h & 0x0F) + (((*h & 0x70) / 16) * 10) ) | (*h & 0x80);
    }
    
    // Convert 12 hour clock to 24 hour clock if necessary
    if (!(registerB & 0x02) && (*h & 0x80)) {
        *h = ((*h & 0x7F) + 12) % 24;
    }
}
