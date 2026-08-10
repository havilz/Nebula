# Mach CPU Architecture Implementation

Folder `kernel/mach/arch/` berisi kode sumber C++ dan Assembly penanganan CPU x86_64.

## Source Tree

```text
kernel/mach/arch/
├── gdt.cpp        # Implementasi GDT Reload
├── idt.cpp        # Implementasi IDT 256 Vector
├── interrupts.cpp # Implementasi ISR & IRQ Dispatcher C++
├── isr.asm        # Assembly Stubs Interupsi IDT
├── pic.cpp        # Implementasi Remap 8259 PIC
└── tss.cpp        # Implementasi Task State Segment 64-bit
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `gdt.cpp` | Pengisian segment selector GDT dan mengeksekusi instruksi `lgdt`. |
| `idt.cpp` | Pengisian gate descriptor IDT dan mengeksekusi instruksi `lidt`. |
| `interrupts.cpp` | Fungsi C++ dispatcher `isr_handler` dan `irq_handler` penangan interupsi. |
| `isr.asm` | Assembly stubs pengubah stack saat interupsi hardware/software dipicu CPU. |
| `pic.cpp` | Mengirimkan byte konfigurasi ICW1-ICW4 ke Port I/O `0x20` & `0xA0`. |
| `tss.cpp` | Mengonfigurasi TSS 64-bit dan memuatnya ke CPU via instruksi `ltr`. |
