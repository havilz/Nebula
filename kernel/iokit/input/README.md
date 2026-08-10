# IOKit Input Drivers Implementation

Folder `kernel/iokit/input/` berisi implementasi driver perangkat masukan.

## Source Tree

```text
kernel/iokit/input/
├── keyboard.cpp  # Implementasi Driver PS/2 Keyboard IRQ 1
└── mouse.cpp     # Implementasi Driver PS/2 Mouse IRQ 12
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `keyboard.cpp` | Pembacaan scancode port `0x60` (IRQ 1) dan penyimpanannya ke ring buffer. |
| `mouse.cpp` | Pembacaan paket data 3-byte IRQ 12 dan perenderan Graphic Arrow Cursor. |
