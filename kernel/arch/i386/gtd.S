# GTD
.section .data
gtd_start:
	.long 0x0
	.long 0x0

gtd_code:
    .byte 0xff       # limit
    .byte 0xff
	.byte 0x0		 # base
	.byte 0x0        # base
    .byte 0x0        # base
    .byte 0b10011010 # flags
    .byte 0b11001111 # flags + limit
    .byte 0x0        # base

gtd_data:
    .byte 0xff       # limit
    .byte 0xff
	.byte 0x0		 # base
	.byte 0x0        # base
    .byte 0x0
    .byte 0b10010010
    .byte 0b11001111
    .byte 0x0

gdt_user_code:
    .byte 0xff
    .byte 0xff
    .byte 0x0
    .byte 0x0
    .byte 0x0
    .byte 0b11111010
    .byte 0b11001111
    .byte 0x0

gdt_user_data:
    .byte 0xff
    .byte 0xff
    .byte 0x0
    .byte 0x0
    .byte 0x0
    .byte 0b11110010
    .byte 0b11001111
    .byte 0x0

gdt_tss:
    .long 0x0
    .long 0x0

gtd_end:

gtd_descriptor:
  .short gtd_end - gtd_start - 1
  .long gtd_start

.global tss
tss:
    .skip 0xFF    

.section .text
.global load_gdt
load_gdt:
	lgdt (gtd_descriptor)
	ljmp $0x08, $load_kernel

load_kernel:
	mov $0x10, %cx
	mov %cx, %ds
	mov %cx, %ss
	mov %cx, %es
	mov %cx, %fs
	mov %cx, %gs
    ret