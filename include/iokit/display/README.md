# IOKit Display Driver Headers

Folder `include/iokit/display/` berisi deklarasi driver tampilan grafis VBE.

## Source Tree

```text
include/iokit/display/
└── vbe.hpp  # Bochs BGA VBE Linear Framebuffer Driver Header
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `vbe.hpp` | Deklarasi pemrograman register Bochs BGA Port `0x1CE`/`0x1CF` dan pengoperasian Linear Framebuffer (LFB 800x600 32-bit). |
