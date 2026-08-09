# Kernel Implementation Directory

Folder `kernel/` berisi seluruh kode sumber implementasi C++ dan Assembly Ring 0 kernel **Nebula OS** (XNU-Hybrid Architecture Model).

## Source Tree

```text
kernel/
├── mach/         # Mach Microkernel Core Implementation (arch/, vm/, sched/)
├── bsd/          # BSD POSIX OS Services Implementation (vfs/, sys/)
├── iokit/        # IOKit Driver Architecture Implementation
├── libkern/      # Libkern C++ Runtime & OSObject Refcounting Implementation
├── libsa/        # Standalone Kernel Runtime & Kext Loader Implementation
├── security/     # Security MAC Framework & Sandbox Implementation
├── pexpert/      # Platform Expert Device Tree Implementation
├── san/          # Memory Sanitizer Hooks Implementation
├── gui/          # Font Engine & Desktop Window Manager Implementation
└── core/         # Kernel Entry Point (kernel.cpp)
```

## Sub-Direktori

* **`mach/`**: Logika CPU abstraction, Physical/Virtual Memory, dan Preemptive Scheduler.
* **`bsd/`**: Logika Virtual File System (VFS), Initrd, dan System Call Handler (`INT 0x80`).
* **`iokit/`**: Logika implementasi driver C++ OOP untuk VBE, Mouse, Keyboard, PIT, dan Serial.
* **`core/`**: Entry point `kernel_main()` yang mengorkestrasi seluruh inisialisasi subsistem kernel.
