.section .text.startup

.global _start
_start: 
    push %edx  # envp
    push %ecx  # envc
    push %ebx  # argv
    push %eax  # argc 

    call initialize_standard_library

    add $16, %esp
    push %ebx # argv
    push %eax # argc

    call _init

    call main

    push %eax
    call exit

.size _start, . - _start
