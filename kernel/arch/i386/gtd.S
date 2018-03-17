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
    .word 104                        # tss size
    .word 0                          # bits 0-15 of tss
    .byte 0                          # bits 16-23 of tss
    .byte 0b10001001                 # p, dpl, type
    .byte 0b00000000                 # G, 0, 0, AVL, limit
    .byte 0                          # bits 24-36 of tss

gtd_end:

gtd_descriptor:
  .short gtd_end - gtd_start - 1
  .long gtd_start

.global tss
tss:
    .long 0
    .long 0    # set to esp
    .long 0x10 # stack segment selector
    .rept 22   # zero out / reserve
        .long 0
    .endr
    .long (104 << 16) # set I/O Map Base Address properly

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

    # Sets up valid tss
    mov $tss, %ecx
    mov $gdt_tss, %edx
    movw %cx, 2(%edx)
    shr $16, %ecx
    movb %cl, 4(%edx) 
    movb %ch, 7(%edx)   

    # Loads tss segment
    mov $0x28, %cx 
    ltr %cx  

    ret