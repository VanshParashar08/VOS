# Makefile for PortfolioOS

CC = clang -target i386-pc-none-elf
AS = clang -target i386-pc-none-elf
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Iinclude -m32 -mno-sse -mno-mmx -mno-sse2
LDFLAGS = -ffreestanding -O2 -nostdlib -fuse-ld=/opt/homebrew/bin/ld.lld -m32

# Source files
C_SOURCES = $(wildcard kernel/*.c drivers/keyboard.c drivers/timer.c drivers/graphics.c drivers/mouse.c drivers/bga.c drivers/rtc.c gui/desktop.c gui/wallpapers.c gui/terminal.c cpu/*.c mm/*.c libc/*.c)
S_SOURCES = boot/boot.S $(wildcard cpu/*.S)

# Object files
OBJ = boot/boot.o $(C_SOURCES:.c=.o) $(filter-out boot/boot.o, $(S_SOURCES:.S=.o))

os.bin: $(OBJ)
	/opt/homebrew/bin/ld.lld -T boot/linker.ld -m elf_i386 -o $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) os.bin initrd.img

initrd.img:
	python3 gen_initrd.py initrd.img fs/resume.txt fs/projects.txt

run: os.bin initrd.img
	qemu-system-i386 -kernel os.bin -initrd initrd.img -rtc base=localtime -display cocoa,full-screen=on,zoom-to-fit=on -monitor stdio

iso: os.bin initrd.img
	mkdir -p isodir/boot/grub
	cp os.bin isodir/boot/PortfolioOS.bin
	cp initrd.img isodir/boot/initrd.img
	echo 'menuentry "PortfolioOS" {' > isodir/boot/grub/grub.cfg
	echo '	multiboot /boot/PortfolioOS.bin' >> isodir/boot/grub/grub.cfg
	echo '	module /boot/initrd.img' >> isodir/boot/grub/grub.cfg
	echo '}' >> isodir/boot/grub/grub.cfg
	grub-mkrescue -o PortfolioOS.iso isodir
