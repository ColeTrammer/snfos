.section .text
.global _start
_start:
    call main
    
    mov $0x02, %eax
    int $0x80
