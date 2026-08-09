# Include Directory - Public Header Files

Folder `include/` merupakan lokasi terpusat seluruh file header (`.hpp` & `.h`) untuk Public API dan subsistem modul **Nebula OS** (XNU-Hybrid Architecture Model).

## Source Tree

```text
include/
├── mach/         # Mach Microkernel Core Headers (CPU, Memory, Scheduler)
├── bsd/          # BSD POSIX OS Services Headers (VFS, Syscall)
├── iokit/        # IOKit Object-Oriented C++ Driver Framework Headers
├── libkern/      # Kernel C++ Utility Library & Port I/O
├── libsa/        # Standalone Kernel Infrastructure & Module Loader
├── security/     # Mandatory Access Control & Security Credentials
├── pexpert/      # Platform Expert Hardware Discovery & Device Tree
├── san/          # Kernel Memory Sanitizer Hooks (KASAN/UBSAN)
├── gui/          # Font Engine & Desktop Window Manager Headers
└── multiboot.h   # Spesifikasi Multiboot 1 Header Standard
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `multiboot.h` | Definisi struktur data Multiboot 1 header yang dikirim oleh bootloader (GRUB/QEMU). |
