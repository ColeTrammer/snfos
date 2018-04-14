.section .text

.global exit
exit:
    # call _fini
    pop %ebx
    mov $2, %eax
    int $0x80