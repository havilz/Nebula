# Nebula OS

**Nebula OS** adalah sistem operasi *bare-metal* buatan sendiri (*custom operating system*) yang dirancang dari nol (*from scratch*) menggunakan **C++20 Freestanding**, **Assembly (x86_64)**, dan **GNU Build System**. Proyek ini bertujuan untuk membangun kernel OS yang modular, cepat, aman, dan kaya fitur dari tingkat bootloader hingga antarmuka grafis (VBE GUI).

---

## 1. Fitur Utama & Spesifikasi Arsitektur

- **Architecture Target**: x86_64 Long Mode (64-bit) dengan kompatibilitas Multiboot 1 / Multiboot 2.
- **Kernel Design**: Hybrid-Modular Architecture (Ring 0 Kernel Space & Ring 3 User Space).
- **Core Memory Management**: Physical Memory Manager (Bitmap PMM), Virtual Memory Manager (PML4 Paging VMM), dan Custom Kernel Heap Allocator (`kmalloc`/`kfree`).
- **Interrupt & Hardware**: GDT, IDT (256 Vector), 8259 PIC Remap, APIC/IOAPIC support, PIT Timer (100Hz), PS/2 Keyboard, dan UART 16550 Serial Logging.
- **Multitasking & Scheduler**: Preemptive Round-Robin / MLFQ Scheduler dengan PCB & TCB context switching.
- **File System & Shell**: Virtual File System (VFS), RAM Disk (Initrd), dan Userland Shell Console.

---

## 2. Struktur Direktori Proyek (XNU-Hybrid Architecture 1-to-1)

```text
Nebula/
├── boot/                        # Bootloader Multiboot 1 & Linker Script
│   ├── x86_64/                  # boot.asm
│   └── linker.ld                # Linker script 32-bit ELF
├── include/                     # Public Header Files (.hpp)
│   ├── mach/                    # Mach Microkernel Core Headers (GDT, IDT, PMM, VMM, Scheduler, IPC)
│   ├── bsd/                     # BSD OS Services Headers (VFS Nodes, Initrd RAM Disk, Syscall INT 0x80)
│   ├── iokit/                   # IOKit C++ Driver Framework Headers (IODevice, VBE, Mouse, Keyboard)
│   ├── libkern/                 # Kernel Utility C++ Library (OSObject Base Class, Port I/O)
│   ├── libsa/                   # Standalone Kernel Infrastructure & Dynamic Kext Module Loader
│   ├── security/                # Mandatory Access Control (MAC Framework, Credentials ucred_t)
│   ├── pexpert/                 # Platform Expert (Device Tree Parser & Early Machine Init)
│   ├── san/                     # Kernel Memory Sanitizer Hooks (KASAN / UBSAN)
│   └── gui/                     # Font Engine & Desktop Window Manager Headers
├── kernel/                      # Implementation Files (.cpp & .asm)
│   ├── mach/                    # Mach Core Implementation (arch/, vm/, sched/)
│   ├── bsd/                     # BSD Services Implementation (vfs/, sys/)
│   ├── iokit/                   # IOKit C++ Drivers Implementation (display/, input/, timer/, serial/)
│   ├── libkern/                 # Libkern C++ Runtime & OSObject Refcounting Implementation
│   ├── libsa/                   # Standalone Kernel Infrastructure Implementation
│   ├── security/                # Security Framework Implementation
│   ├── pexpert/                 # Platform Expert Device Tree Implementation
│   ├── san/                     # Sanitizer Memory Hooks Implementation
│   ├── gui/                     # font.cpp, wm.cpp (Desktop Window Manager Compositor)
│   └── core/                    # kernel.cpp (Kernel Entry Point)
├── docs/                        # Dokumentasi Proyek & Architecture
├── Makefile                     # Updated GNU Make Build Script
└── README.md                    # Dokumentasi Utama Proyek
```

---

## 3. Persiapan & Persyaratan (Prerequisites)

Untuk mengompilasi dan menguji **Nebula OS**, pastikan toolchain berikut terpasang di sistem Anda:

### Windows (MSYS2 UCRT64 Environment):
- **GCC / G++**: Cross-compiler i686/x86_64 (`g++ -m32`)
- **NASM**: Netwide Assembler (`nasm -f elf32`)
- **LLD Linker**: LLVM Linker (`ld.lld`)
- **Make**: `mingw32-make`
- **QEMU**: `qemu-system-i386` atau `qemu-system-x86_64`
- **GDB**: GNU Debugger (`gdb`)

---

## 4. Cara Pengompilan & Eksekusi

### Membangun & Menjalankan Release Mode (QEMU GUI):
```bash
mingw32-make run
```

### Membangun Debug Target & Menjalankan QEMU GDB Server:
```bash
mingw32-make debug
mingw32-make qemu-debug
```

Atau cukup tekan **`F5`** di VS Code untuk memulai sesi visual debugging terintegrasi.

---

## 5. Dokumentasi & Aturan Kontribusi
- **Roadmap & Progress**: [docs/task.md](file:///c:/project/Nebula/docs/task.md)
- **Changelog & Technical Logs**: [docs/CHANGELOG.md](file:///c:/project/Nebula/docs/CHANGELOG.md)
- **Coding Standards & Rules**: [docs/RULES.md](file:///c:/project/Nebula/docs/RULES.md)
- **Architecture Patterns**: [docs/PATTERN.md](file:///c:/project/Nebula/docs/PATTERN.md)
