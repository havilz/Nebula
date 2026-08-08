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

## 2. Struktur Direktori Proyek

```text
Nebula/
├── boot/                   # Bootloader & linker script spesifik arsitektur
│   └── x86_64/             # Kode assembly bootloader & transisi 64-bit Long Mode
├── include/                # Header file C++ (.hpp) terpusat untuk Public API & Subsistem
│   └── kernel/             # Header deklarasi terstruktur (arch, core, drivers, memory, etc.)
│       ├── arch/           # Header spesifikasi CPU (GDT, IDT, PIC, Paging, Context)
│       ├── core/           # Header entry point, panic handler, & logger
│       ├── drivers/        # Header driver perangkat keras (Console, Serial, Timer, Keyboard)
│       ├── memory/         # Header PMM, VMM, Heap, & Allocator
│       ├── process/        # Header PCB, TCB, & Process Manager
│       ├── scheduler/      # Header Scheduler Multitasking
│       ├── filesystem/     # Header VFS & Inode
│       └── syscall/        # Header System Call Interface
├── kernel/                 # Kode sumber implementasi Kernel Ring 0 (.cpp & .asm)
│   ├── core/               # Implementasi kernel main, init, panic handler, & logger
│   ├── arch/               # Logika abstraksi hardware CPU (GDT, IDT, PIC, Interrupts)
│   ├── memory/             # Logika memori fisik (PMM), virtual (VMM), heap, & allocator
│   ├── process/            # Logika manajemen proses, thread, & alokasi PID
│   ├── scheduler/          # Logika scheduler multitasking (Preemptive Round Robin)
│   ├── ipc/                # Logika komunikasi antar proses (Pipe, Shared Memory)
│   ├── filesystem/         # Logika Virtual File System (VFS) & driver filesystem
│   ├── drivers/            # Logika driver perangkat keras (Console, Keyboard, Serial)
│   ├── syscall/            # Logika handler System Call (Ring 3 ke Ring 0)
│   └── runtime/            # Support runtime C++ bare-metal
├── userland/               # Aplikasi & lingkungan pengguna Ring 3 (Init, Shell, Commands)
├── libc/                   # C Standard Library buatan sendiri
├── tests/                  # Pengujian unit, subsistem, & integrasi
├── tools/                  # Alat bantu build system, pembuatan ISO image, & debugging
├── docs/                   # Dokumentasi arsitektur, spesifikasi memori, CHANGELOG.md, RULES.md
│   ├── architecture/       # Dokumentasi gambaran umum arsitektur & alur boot
│   ├── decisions/          # Dokumentasi keputusan arsitektur (ADR)
│   ├── memory/             # Spesifikasi manajemen memori
│   ├── process/            # Spesifikasi proses & thread
│   ├── scheduler/          # Spesifikasi alur algoritma penjadwalan
│   ├── filesystem/         # Spesifikasi Virtual File System
│   └── hardware/           # Spesifikasi antarmuka perangkat keras
├── scripts/                # Skrip otomatisasi build & utilitas
├── config/                 # Konfigurasi sistem & kernel
├── Makefile                # Script otomatisasi kompilasi GNU Make
├── CMakeLists.txt          # Konfigurasi build sistem CMake
└── LICENSE                 # Lisensi open-source project
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
