# Nebula OS

**Nebula OS** adalah Sistem Operasi modular 64-bit (x86_64) yang dibangun dari nol (bare-metal) menggunakan C++ modern dan Assembly. Project ini dirancang untuk menyediakan kernel hybrid yang aman, cepat, dan terstruktur dengan antarmuka grafis (GUI Window Manager) di atasnya.

---

## 1. Fitur Utama & Target

* **Arsitektur x86_64 Native**: Berjalan di atas mode 64-bit Long Mode dengan struktur Paging PML4 4-tingkat.
* **Higher-Half Kernel**: Pemetaan memori terpisah antara area Kernel (`0xFFFF800000000000`) dan Userland (`0x0000000000000000`).
* **C++ Bare-Metal (Freestanding)**: Pemrograman C++ tanpa dependensi OS eksternal (`-ffreestanding -fno-exceptions -fno-rtti`).
* **Manajemen Memori Lengkap**: PMM (Physical Memory Manager Bitmap), VMM (Virtual Memory Manager Paging), dan Heap Allocator (`kmalloc`/`operator new`).
* **Multitasking Preemptif**: Scheduler proses & thread berbasis interupsi timer hardware (PIT/APIC).
* **Virtual File System (VFS)**: Abstraksi akses berkas terpadu.
* **Graphical Desktop & Window Manager**: Render grafis VBE Linear Framebuffer 32-bit color dengan antarmuka jendela interaktif.

---

## 2. Struktur Direktori & Fungsi Folder

```text
Nebula/
├── boot/                   # Bootloader & linker script spesifik arsitektur
│   └── x86_64/             # Kode assembly bootloader & transisi 64-bit Long Mode
├── kernel/                 # Kode sumber utama Kernel Ring 0
│   ├── core/               # Entry point kernel main, init, panic handler, & logger
│   ├── arch/               # Abstraksi hardware CPU (x86_64 GDT, IDT, Paging, Context)
│   ├── memory/             # Manajer memori fisik (PMM), virtual (VMM), heap, & allocator
│   ├── process/            # Manajemen proses, thread, & alokasi PID
│   ├── scheduler/          # Scheduler multitasking & kebijakan penjadwalan (Round Robin, MLFQ)
│   ├── ipc/                # Komunikasi antar proses (Pipe, Channel, Shared Memory)
│   ├── filesystem/         # Virtual File System (VFS), inode, & driver filesystem
│   ├── drivers/            # Driver perangkat keras (Console, Keyboard, Timer, Serial, Storage, Network)
│   ├── syscall/            # Lapisan & handler System Call (Ring 3 ke Ring 0)
│   └── runtime/            # Support runtime C++ bare-metal & konstruktor global
├── userland/               # Aplikasi & lingkungan pengguna Ring 3 (Init, Shell, Commands)
├── libc/                   # C Standard Library buatan sendiri
├── include/                # Header file C++ (.hpp) terpusat untuk kernel & subsistem
├── tests/                  # Pengujian unit, subsistem, & integrasi
├── tools/                  # Alat bantu build system, pembuatan ISO image, & debugging
├── docs/                   # Dokumentasi arsitektur, spesifikasi memori, task.md, RULES.md, & PATTERN.md
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

Untuk membangun dan menjalankan Nebula OS, pastikan perangkat Anda memiliki:

* **Compiler**: GCC / G++ dengan dukungan target `-m64` atau `x86_64-elf-gcc`.
* **Assembler**: NASM (Netwide Assembler).
* **Build Tool**: GNU Make.
* **Emulator**: QEMU (`qemu-system-x86_64`).
* **ISO Creator**: `xorriso` / `grub-mkrescue`.

---

## 4. Cara Compiling & Running

```bash
# Compiling kernel dan langsung menjalankan di QEMU Emulator:
make run

# Membersihkan file objek hasil kompilasi:
make clean
```

---

## 5. Dokumentasi Lengkap

* [Tracking Task & Roadmap Project](docs/task.md)
* [Arsitektur Umum OS](docs/architecture/overview.md)
* [Spesifikasi Alur Booting](docs/architecture/boot_sequence.md)
* [Aturan Penulisan Kode](docs/RULES.md)
* [Pola Desain C++ OS](docs/PATTERN.md)

---

## 6. Lisensi
Dikembangkan di bawah lisensi [MIT License](LICENSE).
