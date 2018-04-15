.section .text

.global exit
exit:
    call _fini
    mov 4(%esp), %ebx
    mov $2, %eax
    int $0x80