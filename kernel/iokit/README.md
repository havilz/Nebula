# IOKit Driver Implementation

Folder `kernel/iokit/` berisi kode sumber implementasi driver perangkat keras C++ berorientasi objek.

## Source Tree

```text
kernel/iokit/
├── iodevice.cpp  # Implementasi Base Class IODevice
├── console/      # Implementasi Driver VGA Text Console
├── display/      # Implementasi Driver Bochs BGA VBE Framebuffer
├── input/        # Implementasi Driver PS/2 Mouse & Keyboard
├── timer/        # Implementasi Driver PIT 8254 Timer
└── serial/       # Implementasi Driver UART 16550 Serial Debug Logger
```

## Sub-Direktori

* **`iodevice.cpp`**: Konstruktor dan destruktor kelas dasar `IODevice`.
* **`display/`**: Pemrograman mode grafis 800x600 32-bit BPP via port `0x1CE`/`0x1CF`.
* **`input/`**: Penanganan interupsi hardware IRQ 1 (Keyboard) dan IRQ 12 (Mouse).
* **`timer/`**: Pemrograman pemicuan tick 100Hz pada PIT 8254.
* **`serial/`**: Pengiriman log debug serial via Port COM1 (`0x3F8`).
