# Task & Roadmap Progress - Nebula OS

Dokumen ini melacak seluruh daftar tugas, status pengembangan, dan roadmap fitur dalam pembangunan **Nebula OS**.

---

## Status Fase Pengembangan

```text
[X] Fase 0: Setup Lingkungan & Arsitektur Project (Selesai 100%)
[X] Fase 1: Bootloader & Bare-Metal Kernel Execution (Selesai 100% - Terverifikasi di QEMU)
[X] Fase 2: GDT, IDT, & Interupsi CPU (ISR & PIC) (Selesai 100% - Terverifikasi di QEMU)
[X] Fase 3: Manajemen Memori (PMM Bitmap, VMM Paging, Heap) (Selesai 100% - Terverifikasi di QEMU)
[ ] Fase 4: Driver Hardware Input & Timer (PIT, PS/2 Keyboard, Serial COM1)
[ ] Fase 5: Multitasking & Scheduler (PCB, TCB, Context Switching)
[ ] Fase 6: Virtual File System (VFS) & System Call (Ring 3 Userland)
[ ] Fase 7: VBE Framebuffer & Antarmuka Grafis (GUI Window Manager)
```

---

## Detail Daftar Tugas (Checklist)

### Fase 0: Setup Lingkungan & Arsitektur Project
- [x] Pembuatan struktur folder modular project.
- [x] Pembuatan dokumentasi arsitektur (`docs/architecture/overview.md`, `docs/architecture/boot_sequence.md`).
- [x] Pembuatan standar penulisan kode (`docs/RULES.md`) dan pola desain (`docs/PATTERN.md`).
- [x] Spesifikasi subsistem memori, proses, scheduler, VFS, dan driver.
- [x] Persiapan lingkungan kompilasi (MSYS2 UCRT64, GCC, NASM, Make, QEMU, LLD Linker).

### Fase 1: Bootloader x86_64 & Hello Bare-Metal Kernel
- [x] Linker Script kernel x86_64 (`boot/linker.ld`).
- [x] Assembly Entry Point & Header Multiboot (`boot/x86_64/boot.asm`).
- [x] Assembly Hardware VGA Driver (`boot/x86_64/vga.asm`).
- [x] Header Driver Layar VGA Console (`include/kernel/drivers/console.hpp`).
- [x] Implementasi Driver Layar VGA Console (`kernel/drivers/console/vga.cpp`).
- [x] Kernel Main C++ Entry Point (`kernel/core/kernel.cpp`).
- [x] Automated Build System (`Makefile`).
- [x] Pengujian kompilasi & eksekusi pertama di QEMU Emulator (Sukses 100%).

### Fase 2: GDT, IDT, & Penanganan Interupsi
- [x] Penyiapan GDT 64-bit Reload & Task State Segment (TSS) (`include/kernel/arch/x86_64/gdt.hpp`, `kernel/arch/x86_64/gdt/gdt.cpp`).
- [x] Penyiapan Interrupt Descriptor Table (IDT 256 Vector) (`include/kernel/arch/x86_64/idt.hpp`, `kernel/arch/x86_64/idt/idt.cpp`).
- [x] Assembly ISR Stubs & Dispatcher Interupsi C++ (`kernel/arch/x86_64/interrupts/isr.asm`, `include/kernel/arch/x86_64/interrupts.hpp`, `kernel/arch/x86_64/interrupts/interrupts.cpp`).
- [x] Remapping 8259 PIC (IRQ 0-15 ke Vector IDT 32-47) (`include/kernel/arch/x86_64/pic.hpp`, `kernel/arch/x86_64/interrupts/pic.cpp`).

### Fase 3: Manajemen Memori
- [x] Physical Memory Manager (PMM) berbasis Bitmap (`include/kernel/memory/pmm.hpp`, `kernel/memory/pmm.cpp`).
- [x] Virtual Memory Manager (VMM) Pemetaan Page Table PML4 (`include/kernel/memory/vmm.hpp`, `kernel/memory/vmm.cpp`).
- [x] Kernel Heap Allocator (`kmalloc`/`kfree`) (`include/kernel/memory/heap.hpp`, `kernel/memory/heap.cpp`).
- [x] Overloading Operator Global C++ `new` dan `delete` (`kernel/memory/kheap.cpp`).

### Fase 4: Driver Hardware Input & Timer
- [ ] Driver PIT Timer (100 Hz Tick Interrupt IRQ 0).
- [ ] Driver PS/2 Keyboard (IRQ 1 Scancode Ring Buffer).
- [ ] Driver Serial Port COM1 (UART 16550) untuk Log Debugging.

### Fase 5: Proses & Scheduler Preemptif
- [ ] Struktur Data Process Control Block (PCB) & Thread Control Block (TCB).
- [ ] Penukaran Konteks CPU Assembly (`asm_context_switch`).
- [ ] Algoritma Preemptive Round-Robin Scheduler.
- [ ] System Call Infrastructure (`syscall`/`sysret` & INT 0x80).

### Fase 6: Virtual File System (VFS) & Userland
- [ ] Abstraksi VFS VNode & Mount Table.
- [ ] Driver Memori Virtual RAM Disk (Initrd).
- [ ] Transisi Ring 0 Kernel ke Ring 3 User Mode.

### Fase 7: VBE Framebuffer & GUI Window Manager
- [ ] Modus Grafis VBE High-Resolution (1024x768 32-bit Color).
- [ ] Compositor Window Manager & Render Font.
