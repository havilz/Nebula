# IOKit Console Driver Implementation

Folder `kernel/iokit/console/` berisi implementasi driver konsol teks.

## Source Tree

```text
kernel/iokit/console/
└── vga.cpp  # Implementasi VGA Text Mode Driver
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `vga.cpp` | Pencetakan karakter berwarna ke memori video teks `0xB8000` dan kontrol kursor. |
