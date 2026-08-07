; multiboot header & CPU entry point
[BITS 32]

MB_ALIGN     equ 1 << 0
MB_MEMINFO   equ 1 << 1
MB_FLAGS     equ MB_ALIGN | MB_MEMINFO ; 0x3 (Standard Multiboot 1 ELF32)
MB_MAGIC     equ 0x1BADB002
MB_CHECKSUM  equ -(MB_MAGIC + MB_FLAGS)

global _start
extern kernel_main

section .multiboot
align 4
multiboot_header:
    dd MB_MAGIC
    dd MB_FLAGS
    dd MB_CHECKSUM

section .bss
align 16
stack_bottom:
    resb 16384 ; 16 kib stack memory
stack_top:

section .text
_start:
    cli                 ; disable interrupts immediately - IF state is undefined per Multiboot spec
    mov esp, stack_top
    and esp, -16        ; Align stack to 16-byte boundary for System V ABI
    sub esp, 12         ; 12 bytes padding so ESP + return address (4) is 16-byte aligned

    call kernel_main

.hang:
    cli
    hlt
    jmp .hang