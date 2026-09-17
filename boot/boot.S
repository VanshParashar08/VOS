/* boot.S - Entry point for the PortfolioOS kernel */

/* Constants for Multiboot header */
.set ALIGN,       1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,     1<<1             /* provide memory map */
.set VIDEO_MODE,  1<<2             /* request video mode */
.set FLAGS,       ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MAGIC,       0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM,    -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */

/* 
Declare a header as in the Multiboot Standard. 
We put this in a special section so we can force it to be at the very front of the kernel.
*/
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
.long 0 /* header_addr */
.long 0 /* load_addr */
.long 0 /* load_end_addr */
.long 0 /* bss_end_addr */
.long 0 /* entry_addr */
.long 0 /* mode_type: 0 for linear graphics */
.long 800 /* width */
.long 600 /* height */
.long 32  /* depth */

/*
Allocate a small stack in the .bss section.
*/
.section .bss
.align 16
stack_bottom:
.skip 16384 /* 16 KiB */
stack_top:

/*
The linker script specifies _start as the entry point to the kernel.
*/
.section .text
.global _start
.type _start, @function
_start:
	/*
	To set up a stack, we set the esp register to point to the top of our
	stack (as it grows downwards on x86).
	*/
	mov $stack_top, %esp

	/*
	Push multiboot info and magic number onto the stack to pass to kernel_main
	*/
	push %eax
	push %ebx

	/*
	Call the global C function kernel_main.
	*/
	.extern kernel_main
	call kernel_main

	/*
	If the system has nothing more to do, put the computer into an infinite loop.
	*/
	cli
1:	hlt
	jmp 1b

/*
Set the size of the _start symbol to the current location '.' minus its start.
*/
.size _start, . - _start
