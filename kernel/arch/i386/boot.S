# Declare constants for the multiboot header.
.set ALIGN,    1 << 0             # align loaded modules on page boundaries
.set MEMINFO,  1 << 1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

.set KERNEL_VIRTUAL_BASE, 0xC0000000
.set KERNEL_PAGE_NUMBER, (KERNEL_VIRTUAL_BASE >> 22)

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Page Table
.section .data
.align 0x1000
boot_page_directory:
	.long 0x00000083                                         # 4 MB direct translation page 
	.rept (KERNEL_PAGE_NUMBER - 1) 
		.long 0
	.endr
	.long (kernel_page_table - KERNEL_VIRTUAL_BASE + 0x03)   # Maps to kernel page table
	.rept (1024 - KERNEL_PAGE_NUMBER - 2)                    # Goes until 1 away from the end 
		.long 0
	.endr
	.long (boot_page_directory - KERNEL_VIRTUAL_BASE + 0x03) # Map PTE to itself

.align 0x1000
kernel_page_table:
	.rept 1024
		.long 0
	.endr

.align 0x1000
.global page_bitmap
page_bitmap:
	.skip 0x1000 * 32

# Reserve a stack for the initial thread.
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.global start
.set start, _start - KERNEL_VIRTUAL_BASE

# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	# Set up kernel page table
	mov $(kernel_page_table - KERNEL_VIRTUAL_BASE), %ecx
	mov $0x03, %edx
	add_mapping:
		cmp $kernel_physical_end, %edx
		jge enbable_paging
		mov %edx, (%ecx)
		add $0x1000, %edx
		add $0x4, %ecx
		jmp add_mapping

enbable_paging:
	# Enable paging
	mov $(boot_page_directory - KERNEL_VIRTUAL_BASE), %ecx
	mov %ecx, %cr3

	mov %cr4, %ecx
	or $0x00000010, %ecx  # Sets PSE
	mov %ecx, %cr4

	mov %cr0, %ecx
	or $0x80000000, %ecx  # Sets PG
	mov %ecx, %cr0

	lea (higher_half), %ecx
	jmp *%ecx

higher_half:
	movw $0, (boot_page_directory)
	invlpg (0)

	# Set up stack
	movl $stack_top, %esp

	# Load GTD
	call load_gdt

	# Push arguments onto stack (_init modifies eax)
	push %ebx
	push %eax

	# Call the global constructors.
	call _init

	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
1:	hlt
	jmp 1b
.size _start, . - _start