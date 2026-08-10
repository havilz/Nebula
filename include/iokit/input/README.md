# IOKit Input Driver Headers

Folder `include/iokit/input/` berisi deklarasi driver perangkat masukan (keyboard dan mouse).

## Source Tree

```text
include/iokit/input/
├── keyboard.hpp  # PS/2 Keyboard Driver Header
└── mouse.hpp     # PS/2 Mouse Driver Header
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `keyboard.hpp` | Deklarasi pembacaan scancode IRQ 1 dan konversi karakter US QWERTY. |
| `mouse.hpp` | Deklarasi pembacaan paket 3-byte IRQ 12 dan pembaruan posisi kursor mouse grafis. |
