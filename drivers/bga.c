#include "../include/bga.h"
#include "../include/ports.h"

#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA  0x01CF

#define VBE_DISPI_INDEX_ID           0
#define VBE_DISPI_INDEX_XRES         1
#define VBE_DISPI_INDEX_YRES         2
#define VBE_DISPI_INDEX_BPP          3
#define VBE_DISPI_INDEX_ENABLE       4
#define VBE_DISPI_INDEX_BANK         5
#define VBE_DISPI_INDEX_VIRT_WIDTH   6
#define VBE_DISPI_INDEX_VIRT_HEIGHT  7
#define VBE_DISPI_INDEX_X_OFFSET     8
#define VBE_DISPI_INDEX_Y_OFFSET     9

#define VBE_DISPI_DISABLED     0x00
#define VBE_DISPI_ENABLED      0x01
#define VBE_DISPI_LFB_ENABLED  0x40

static void bga_write_register(uint16_t index, uint16_t value) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    outw(VBE_DISPI_IOPORT_DATA, value);
}

void bga_init(uint32_t width, uint32_t height, uint32_t bpp) {
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write_register(VBE_DISPI_INDEX_XRES, width);
    bga_write_register(VBE_DISPI_INDEX_YRES, height);
    bga_write_register(VBE_DISPI_INDEX_BPP, bpp);
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}

// In QEMU, the Bochs VBE LFB is usually at PCI BAR0 of the VGA device.
// Hardcoding 0xFD000000 works for modern QEMU.
// For older versions, it might be 0xE0000000.
// Let's do a quick PCI read just to be safe.
static uint32_t pci_config_read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address = (uint32_t)((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
    outl(0xCF8, address);
    return inl(0xCFC);
}

uint32_t bga_get_lfb(void) {
    // Scan PCI bus 0 for QEMU VGA
    for (uint8_t slot = 0; slot < 32; slot++) {
        uint32_t vendor_device = pci_config_read_dword(0, slot, 0, 0);
        if (vendor_device == 0x11111234 || vendor_device == 0xbeef80EE) {
            // Found QEMU VGA or VirtualBox VGA
            uint32_t bar0 = pci_config_read_dword(0, slot, 0, 0x10);
            return bar0 & 0xFFFFFFF0; // Mask out the flag bits
        }
    }
    // Fallback for QEMU stdvga
    return 0xFD000000;
}
