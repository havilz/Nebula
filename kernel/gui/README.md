# GUI Engine Implementation

Folder `kernel/gui/` berisi kode sumber implementasi mesin tampilan grafis dan compositor.

## Source Tree

```text
kernel/gui/
├── font.cpp  # Implementasi Bitmap Font Engine 8x16
└── wm.cpp    # Implementasi Desktop Window Manager Compositor
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `font.cpp` | Merender piksel karakter bitmap 8x16 berwarna pada koordinat layar GUI. |
| `wm.cpp` | Merender wallpaper Deep Space, Taskbar, Jam Uptime, Titlebar, dan mengelola window dragging. |
