#include "../include/gdt.h"
#include "../include/idt.h"
#include "../include/timer.h"
#include "../include/keyboard.h"
#include "../include/pmm.h"
#include "../include/multiboot.h"
#include "../include/graphics.h"
#include "../include/mouse.h"
#include "../include/desktop.h"
#include "../include/terminal.h"
#include <stdint.h>

extern void isr_install(void);
extern int strcmp(const char*, const char*);

void kernel_main(multiboot_info_t* mbd, uint32_t magic) {
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        // Not booted by a Multiboot compliant bootloader
        return;
    }

    // Install GDT
    init_gdt();

    // Install IDT and ISRs
    init_idt();
    isr_install();

    // Initialize Memory
    init_pmm(0x100000 * 128); // Assume 128MB RAM for now
    pmm_init_region(0x100000, 0x100000 * 128 - 0x100000);

    // Initialize devices
    init_timer(50);
    init_keyboard();

    // Set up BGA for 800x600x32
    #include "../include/bga.h"
    bga_init(800, 600, 32);
    uint32_t lfb = bga_get_lfb();

    // Initialize our basic graphics driver with the BGA framebuffer
    graphics_init(lfb, 800, 600, 800 * 4, 32);

    // Initialize mouse
    init_mouse();
    
    // Enable interrupts now that everything is set up
    __asm__ volatile("sti");

    // Initialize the desktop UI
    // desktop_init() is called inside desktop_render() when mouse moves,
    // but we can force a first frame draw here.
    extern void desktop_init(void);
    extern void desktop_render(void);
    extern void show_loading_screen(void);
    
    show_loading_screen();
    
    // Initialize filesystem from Initrd if present
    #include "../include/fs.h"
    if (mbd->flags & 0x08) {
        if (mbd->mods_count > 0) {
            multiboot_module_t* mod = (multiboot_module_t*)mbd->mods_addr;
            fs_init(mod->mod_start);
        }
    }
    
    terminal_init();
    desktop_init();

    while (1) {
        desktop_render();
        // Wait for interrupt
        __asm__ volatile("hlt");
    }
}
