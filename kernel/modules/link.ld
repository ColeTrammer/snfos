SECTIONS
{
    . = 0;                 /* relocate to address 0 */

    .text ALIGN(4K):
    {
        start.o(.text)     /* include the .text section of start.o */
        *(.text)           /* include all other .text sections */
    }

    .rodata ALIGN(4K):
    {
        *(.rodata*)
    }

    .data ALIGN(4K):
    {
        *(.data)
    }
}