ENTRY(_start)

SECTIONS
{
    . = 0x100000;

    .multiboot :
    {
        KEEP(*(.multiboot))
    }

    .text :
    {
        *(.text*)
        *(.rodata*)
        *(.rdata*)
        *(.data*)
        *(.bss*)
        *(COMMON)
    }
}