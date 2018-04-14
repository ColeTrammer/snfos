.section .text

.global _start
_start: 
    push %edx  # envp
    push %ecx  # envc
    push %ebx  # argv
    push %eax  # argc 

    call initialize_standard_library

    # call _init

    call main

    push %ecx
    call exit

.size _start, . - _start
