# Task & Roadmap Progress - Nebula OS

Dokumen ini melacak seluruh daftar tugas, status pengembangan, dan roadmap fitur dalam pembangunan **Nebula OS**.

---

## Status Fase Pengembangan

```text
[X] Fase 0: Setup Lingkungan & Arsitektur Project (Selesai 100%)
[X] Fase 1: Bootloader & Bare-Metal Kernel Execution (Selesai 100% - Terverifikasi di QEMU)
[X] Fase 2: GDT, IDT, & Interupsi CPU (ISR & PIC) (Selesai 100% - Terverifikasi di QEMU)
[X] Fase 3: Manajemen Memori (PMM Bitmap, VMM Paging, Heap) (Selesai 100% - Terverifikasi di QEMU)
[X] Fase 4: Driver Hardware Input & Timer (PIT, PS/2 Keyboard, Serial) (Selesai 100% - Terverifikasi di QEMU)
[X] Fase 5: Multitasking & Preemptive Scheduler (PCB, TCB, TSS 64-bit, Time Slicing) (Selesai 100% - Terverifikasi di QEMU)
[X] Fase 6: Virtual File System (VFS), Initrd RAM Disk, & Ring 3 System Calls (INT 0x80) (Selesai 100% - Terverifikasi di QEMU)
[X] Fase 7: VBE Framebuffer & Antarmuka Grafis (GUI Window Manager) (Selesai 100% - Terverifikasi di QEMU)
[X] Fase 8: Refactoring Kernel ke Arsitektur XNU-Hybrid (Mach Core, BSD Services, & IOKit Drivers) (Selesai 100% - Terverifikasi di QEMU)
[X] Fase 9: Driver Storage ATA/AHCI & System Berkas Permanen (FAT32/Ext2) (Selesai 100% - Terverifikasi di QEMU)
[X] Fase 10: ELF Executable Loader, Standard C Library (libnebula), & POSIX Compatibility Layer (Selesai 100% - Terverifikasi di QEMU)
[ ] Fase 11: Advanced GUI Toolkit, Window Resizing, & Desktop Application Suite
[X] Fase 12: Network Driver (E1000/RTL8139) & TCP/IP Protocol Stack (Selesai 100% - Terverifikasi di QEMU)
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
- [x] Header Driver Layar VGA Console (`include/iokit/console/console.hpp`).
- [x] Implementasi Driver Layar VGA Console (`kernel/iokit/console/vga.cpp`).
- [x] Kernel Main C++ Entry Point (`kernel/core/kernel.cpp`).
- [x] Automated Build System (`Makefile`).
- [x] Pengujian kompilasi & eksekusi pertama di QEMU Emulator (Sukses 100%).

### Fase 2: GDT, IDT, & Penanganan Interupsi
- [x] Penyiapan GDT Reload (`include/mach/arch/gdt.hpp`, `kernel/mach/arch/gdt.cpp`).
- [x] Penyiapan Interrupt Descriptor Table (IDT 256 Vector) (`include/mach/arch/idt.hpp`, `kernel/mach/arch/idt.cpp`).
- [x] Assembly ISR Stubs & Dispatcher Interupsi C++ (`kernel/mach/arch/isr.asm`, `include/mach/arch/interrupts.hpp`, `kernel/mach/arch/interrupts.cpp`).
- [x] Remapping 8259 PIC (IRQ 0-15 ke Vector IDT 32-47) (`include/mach/arch/pic.hpp`, `kernel/mach/arch/pic.cpp`).

### Fase 3: Manajemen Memori
- [x] Physical Memory Manager (PMM) berbasis Bitmap (`include/mach/vm/pmm.hpp`, `kernel/mach/vm/pmm.cpp`).
- [x] Virtual Memory Manager (VMM) Pemetaan Page Table (`include/mach/vm/vmm.hpp`, `kernel/mach/vm/vmm.cpp`).
- [x] Kernel Heap Allocator (`kmalloc`/`kfree`) (`include/mach/vm/heap.hpp`, `kernel/mach/vm/heap.cpp`).
- [x] Overloading Operator Global C++ `new` dan `delete` (`kernel/mach/vm/kheap.cpp`).

### Fase 4: Driver Hardware Input & Timer
- [x] Driver PIT Timer 8254 (100 Hz Tick Interrupt IRQ 0) (`include/iokit/timer/pit.hpp`, `kernel/iokit/timer/pit.cpp`).
- [x] Driver PS/2 Keyboard (IRQ 1 Scancode Ring Buffer & US QWERTY Table) (`include/iokit/input/keyboard.hpp`, `kernel/iokit/input/keyboard.cpp`).
- [x] Driver Port I/O Abstraksi Terpusat (`include/libkern/io.hpp`).
- [x] Interactive Shell Echo & Real-time Formatting (`kernel/core/kernel.cpp`).

### Fase 5: Proses & Scheduler Preemptif
- [x] Header Struktur PML4 4-Level 64-bit (`include/mach/arch/paging64.hpp`).
- [x] Struct Task State Segment (TSS 64-bit 104 byte) & Manager (`include/mach/arch/tss.hpp`, `kernel/mach/arch/tss.cpp`).
- [x] Struktur Data Process Control Block (PCB) & Thread Control Block (TCB) (`include/mach/sched/process.hpp`).
- [x] Algoritma Preemptive Round-Robin Scheduler (`include/mach/sched/scheduler.hpp`, `kernel/mach/sched/scheduler.cpp`).
- [x] Pengujian Concurrent Execution Kernel Thread Alpha & Beta di QEMU (`kernel/core/kernel.cpp`).

### Fase 6: Virtual File System (VFS) & Userland
- [x] Abstraksi VFS VNode & Operation Table (`include/bsd/vfs/vfs.hpp`, `kernel/bsd/vfs/vfs.cpp`).
- [x] Driver Memori Virtual RAM Disk (Initrd) (`include/bsd/vfs/initrd.hpp`, `kernel/bsd/vfs/initrd.cpp`).
- [x] Infrastruktur System Call IDT Vector 128 / `INT 0x80` (`include/bsd/sys/syscall.hpp`, `kernel/bsd/sys/syscall.cpp`).
- [x] Interface Assembly User Mode Switch (`include/mach/arch/usermode.hpp`).
- [x] Pengujian VFS Reading (`/initrd/hello.txt`) & System Call (`SYS_WRITE`) di QEMU (`kernel/core/kernel.cpp`).

### Fase 7: VBE Framebuffer & GUI Window Manager
- [x] Driver VBE Linear Framebuffer (LFB) & Double Buffering Backbuffer RAM (`include/iokit/display/vbe.hpp`, `kernel/iokit/display/vbe.cpp`).
- [x] Bitmap Font Engine 8x16 Pixel (`include/gui/font.hpp`, `kernel/gui/font.cpp`).
- [x] Driver PS/2 Mouse IRQ 12 & Graphic Cursor Renderer (`include/iokit/input/mouse.hpp`, `kernel/iokit/input/mouse.cpp`).
- [x] Desktop Compositor & GUI Window Manager (`include/gui/wm.hpp`, `kernel/gui/wm.cpp`).
- [x] Pengujian Desktop Wallpaper, Taskbar Uptime Clock, Jendela System Monitor, & Jendela Interactive Terminal GUI di QEMU (`kernel/core/kernel.cpp`).

### Fase 8: Refactoring Kernel ke Arsitektur XNU-Hybrid 1-to-1 (Mach Core, BSD Services, IOKit Drivers, Libkern, Libsa, Security, Pexpert, San)
- [x] Penataan Layer Kernel Mach-Core Primitives (`kernel/mach/` -> Thread, Scheduler, PMM/VMM Paging, IPC Ports).
- [x] Penataan Layer BSD OS Services (`kernel/bsd/` -> VFS, Process Tree, Syscall Handler, Signal/FD Table).
- [x] Framework Driver C++ Berorientasi Objek IOKit/DriverKit (`kernel/iokit/` -> `IODevice` Base Class, Lifecycle Management).
- [x] Modul Support Kernel Runtime & Security (`libkern`, `libsa`, `security`, `pexpert`, `san`).
- [x] Pembaruan Build System Makefile, VS Code Intellisense, & Pengujian Eksekusi di QEMU (Sukses 100%).

### Fase 9: Driver Storage ATA/AHCI & System Berkas Permanen (FAT32/Ext2)
- [x] Driver Controller Storage ATA / AHCI (Primary Bus LBA28 PIO Sector Read/Write 512-byte).
- [x] Parser MBR / GPT Partition Table (Sector 0 Boot Signature 0xAA55 & FAT32 LBA Scanner).
- [x] Driver Filesystem FAT32 / Ext2 (BPB Parser, Cluster Calculation, Root VNode Mounting `/fat32/`).

### Fase 10: ELF Executable Loader, Standard C Library (libnebula), & POSIX Compatibility Layer
- [x] ELF32 / ELF64 Executable Binary Loader (`elf.hpp`/`elf.cpp` PT_LOAD Segment Parser).
- [x] Implementation Userland Standard C Library (`userland/libc/` -> `<stdio.h>`, `<stdlib.h>`, `<string.h>`, `<unistd.h>`, `crt0.asm`).
- [x] POSIX System Call Layer Gate Vector 128 (`SYS_EXEC`, `SYS_FORK`, `SYS_EXIT`, `SYS_MALLOC`, `SYS_WRITE`).
- [x] Format Paket Aplikasi Native Nebula OS (`.app` / `.neb` Bundle Structure in `bundle.hpp`).

### Fase 11: Advanced GUI Toolkit, Window Resizing, & Desktop Application Suite
- [ ] Library Component GUI Widget (Button, TextBox, Label, ProgressBar, MenuBar, Dropdown).
- [ ] Window Manager Interaktif (Window Resizing & Window State Maximize/Minimize).
- [x] Optimasi Rendering GUI Engine (Offscreen Wallpaper Cache, Fast Mouse Cursor Restore Buffer 16x16, Dirty Rectangles `VBE::swap_rect`).
- [ ] Desktop Application Suite (Nebula Terminal App, File Explorer App, Text Editor App, System Monitor App).

### Fase 12: Network Driver (E1000/RTL8139) & TCP/IP Protocol Stack
- [x] Driver Ethernet Network Card (Intel E1000 PCI MMIO, MAC Address 6-byte, RX/TX Ring Descriptors).
- [x] TCP/IP Protocol Stack (Ethernet II, ARP, IPv4 `10.0.2.15`, ICMP Ping Echo Reply, UDP, TCP).
- [x] BSD Network Socket System Calls (`sys_socket`, `sys_bind`).
