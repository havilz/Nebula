ENTRY(_start)

SECTIONS
{
    . = 0x100000;

    .text :
    {
        KEEP(*(.multiboot))
        *(.text*)
        *(.rodata*)
        *(.rdata*)
        *(.data*)
        *(.bss*)
        *(COMMON)
    }
}