SECTIONS
{
    . = 0;                 /* relocate to address 0 */

    .text ALIGN(4):
    {
        start.o(.text)     /* include the .text section of start.o */
        *(.text)           /* include all other .text sections */
    }

    .rodata ALIGN(4):
    {
        *(.rodata*)
    }

    .data ALIGN(4):
    {
        *(.data)
    }
}