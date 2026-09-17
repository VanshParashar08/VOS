#include "../include/idt.h"
#include "../include/string.h"
#include <stdint.h>
#include <stddef.h>

struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idt_reg;

extern void idt_flush(uint32_t);

void set_idt_gate(int n, uint32_t handler) {
    idt[n].base_low = (handler & 0xFFFF);
    idt[n].sel = 0x08; /* Code segment in GDT */
    idt[n].always0 = 0;
    idt[n].flags = 0x8E; /* 10001110b : present, ring 0, 32-bit interrupt gate */
    idt[n].base_high = (handler >> 16) & 0xFFFF;
}

void init_idt(void) {
    idt_reg.limit = IDT_ENTRIES * sizeof(struct idt_entry) - 1;
    idt_reg.base = (uint32_t)&idt;

    memset(&idt, 0, sizeof(struct idt_entry) * IDT_ENTRIES);

    idt_flush((uint32_t)&idt_reg);
}
