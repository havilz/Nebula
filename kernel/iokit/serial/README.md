# IOKit Serial Driver Implementation

Folder `kernel/iokit/serial/` berisi implementasi driver serial COM1.

## Source Tree

```text
kernel/iokit/serial/
└── serial.cpp  # Implementasi UART 16550 Serial Debug Logger
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `serial.cpp` | Inisialisasi Port COM1 (`0x3F8`) dan fungsi pemancaran string log debug. |
