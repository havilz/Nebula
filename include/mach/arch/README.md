# Mach CPU Architecture Headers

Folder `include/mach/arch/` berisi deklarasi header spesifik untuk arsitektur CPU x86_64.

## Source Tree

```text
include/mach/arch/
├── gdt.hpp        # Global Descriptor Table Header
├── idt.hpp        # Interrupt Descriptor Table Header
├── interrupts.hpp # Interrupt Handling & ISR Dispatcher Header
├── pic.hpp        # 8259 Programmable Interrupt Controller Header
├── tss.hpp        # Task State Segment 64-bit Header
├── paging64.hpp   # Structural Header PML4 Paging 64-bit
└── usermode.hpp   # User Mode Ring 3 Switch Header
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `gdt.hpp` | Deklarasi antarmuka pengisian segment GDT (Kernel Code/Data, User Code/Data, TSS). |
| `idt.hpp` | Deklarasi antarmuka pengisian 256 vector Interrupt Descriptor Table. |
| `interrupts.hpp` | Deklarasi fungsi inisialisasi interupsi dan dispatcher C++ `isr_handler`/`irq_handler`. |
| `pic.hpp` | Deklarasi fungsi remapping 8259 PIC (IRQ 0-15 ke Vector 32-47). |
| `tss.hpp` | Deklarasi struktur data Task State Segment 64-bit (104 byte). |
| `paging64.hpp` | Deklarasi struktur data 4-level PML4 Paging. |
| `usermode.hpp` | Deklarasi antarmuka switching dari Ring 0 Kernel Mode ke Ring 3 User Mode. |
