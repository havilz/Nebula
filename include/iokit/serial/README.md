# IOKit Serial Driver Headers

Folder `include/iokit/serial/` berisi deklarasi driver komunikasi serial UART.

## Source Tree

```text
include/iokit/serial/
└── serial.hpp  # UART 16550 Serial Debug Logger Header
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `serial.hpp` | Deklarasi pengiriman karakter dan string ke Port COM1 (`0x3F8`) untuk log debug. |
