#include "../include/timer.h"
#include "../include/isr.h"
#include "../include/ports.h"

volatile uint32_t tick = 0;

static void timer_callback(registers_t *regs) {
    (void)regs;
    tick++;
}

void init_timer(uint32_t freq) {
    // Register our timer callback.
    register_interrupt_handler(32, timer_callback);

    // The value we send to the PIT is the value to divide it's input clock
    // (1193180 Hz) by, to get our required frequency.
    uint32_t divisor = 1193180 / freq;

    // Send the command byte.
    outb(0x43, 0x36);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );

    // Send the frequency divisor.
    outb(0x40, l);
    outb(0x40, h);
}

void sleep(uint32_t ms) {
    uint32_t start = tick;
    uint32_t ticks_to_wait = (ms * 50) / 1000;
    if (ticks_to_wait == 0) ticks_to_wait = 1;
    while (tick < start + ticks_to_wait) {
        __asm__ volatile("hlt");
    }
}
