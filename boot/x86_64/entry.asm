; paging 4-level & transition to 64-bit long mode
[BITS 32]

section .bss
align 4096
pml4_table:
    resb 4096 ; PML4 table
pdpt_table:
    resb 4096 ; PDPT table
page_directory:
    resb 4096 ; Page Directory table

section .rodata
gdt64:
    dq 0 ; null descriptor
.code_segment: equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53) ; 64-bit code segment
.data_segment: equ $ - gdt64
    dq (1 << 44) | (1 << 47) | (1 << 41) ; 64-bit data segment
.pointer:
    dw $ - gdt64 - 1
    dd gdt64 ; 32-bit base address for LGDT in 32-bit mode

section .text
global setup_page_tables
global enable_paging_and_long_mode
extern kernel_main

setup_page_tables:
    mov eax, pdpt_table
    or eax, 0b11 ; present + writable
    mov [pml4_table], eax
    mov [pml4_table + 511 * 8], eax ; higher-half kernel mapping

    mov eax, page_directory
    or eax, 0b11
    mov [pdpt_table], eax

    mov eax, 0b10000011 ; 2MB huge page identity map
    mov [page_directory], eax
    ret

enable_paging_and_long_mode:
    mov eax, pml4_table
    mov cr3, eax ; load PML4 to CR3

    mov eax, cr4
    or eax, 1 << 5 ; enable PAE
    mov cr4, eax

    mov ecx, 0xC0000080 ; EFER MSR
    rdmsr
    or eax, 1 << 8 ; enable long mode (LME)
    wrmsr

    mov eax, cr0
    or eax, 1 << 31 ; enable paging
    mov cr0, eax

    lgdt [gdt64.pointer]
    jmp gdt64.code_segment:kernel_entry64

[BITS 64]
kernel_entry64:
    mov ax, gdt64.data_segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    call kernel_main

.loop:
    cli
    hlt
    jmp .loop