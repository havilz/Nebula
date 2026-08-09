# GUI Engine Headers

Folder `include/gui/` berisi deklarasi antarmuka mesin antarmuka grafis (Font Engine dan Window Manager).

## Source Tree

```text
include/gui/
├── font.hpp  # Bitmap Font Engine Header
└── wm.hpp    # Desktop Window Manager Compositor Header
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `font.hpp` | Deklarasi perenderan karakter dan string bitmap 8x16 piksel. |
| `wm.hpp` | Deklarasi struktur jendela GUI, pengelola event mouse, dan perenderan elemen desktop. |
