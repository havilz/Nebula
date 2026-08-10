# IOKit Display Driver Implementation

Folder `kernel/iokit/display/` berisi implementasi driver tampilan VBE.

## Source Tree

```text
kernel/iokit/display/
└── vbe.cpp  # Implementasi Driver Bochs BGA VBE Linear Framebuffer
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `vbe.cpp` | Pemrograman register BGA Port `0x1CE`/`0x1CF` dan perenderan piksel pada LFB `0xFD000000`. |
