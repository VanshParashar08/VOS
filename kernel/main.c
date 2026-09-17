#include <stdint.h>
#include <stddef.h>
#include "../include/gdt.h"
#include "../include/idt.h"
#include "../include/timer.h"
#include "../include/keyboard.h"
#include "../include/pmm.h"
#include "../include/multiboot.h"
#include "../include/graphics.h"
#include "../include/bga.h"
#include "../include/mouse.h"
#include "../include/desktop.h"
#include "../include/terminal.h"
#include "../include/fs.h"
#include "../include/string.h"

extern void isr_install(void);

void kernel_main(multiboot_info_t* mbd, uint32_t magic) {
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        // Not booted by a Multiboot compliant bootloader
        return;
    }

    // Install Global Descriptor Table (GDT)
    init_gdt();

    // Install Interrupt Descriptor Table (IDT) and ISRs
    init_idt();
    isr_install();

    // Initialize Physical Memory Manager (PMM)
    // Reserves lower 1MB for BIOS/hardware, marks available RAM in bitmap
    init_pmm(0x100000 * 128); // 128MB RAM initialization
    pmm_init_region(0x100000, 0x100000 * 128 - 0x100000);

    // Initialize core hardware timers and input
    init_timer(50);
    init_keyboard();

    // Set up Bochs Graphics Adapter (BGA) for 800x600 @ 32bpp
    bga_init(800, 600, 32);
    uint32_t lfb = bga_get_lfb();

    // Initialize 2D graphics framebuffer subsystem
    graphics_init(lfb, 800, 600, 800 * 4, 32);

    // Initialize PS/2 mouse driver
    init_mouse();
    
    // Enable hardware interrupts
    __asm__ volatile("sti");

    // Display initial loading sequence
    show_loading_screen();
    
    // Initialize Virtual Ramdisk (initrd) filesystem if multiboot module is passed
    if (mbd->flags & 0x08) {
        if (mbd->mods_count > 0) {
            multiboot_module_t* mod = (multiboot_module_t*)mbd->mods_addr;
            fs_init(mod->mod_start);
        }
    }
    
    // Initialize terminal emulator and desktop window manager
    terminal_init();
    desktop_init();

    // Main event and render loop
    while (1) {
        desktop_render();
        __asm__ volatile("hlt");
    }
}
