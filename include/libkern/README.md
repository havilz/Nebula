# Libkern C++ Utility Headers

Folder `include/libkern/` berisi deklarasi library utilitas C++ tingkat kernel dan pembungkus Port I/O.

## Source Tree

```text
include/libkern/
├── libkern.hpp  # OSObject Base Class & Runtime Utilities Header
└── io.hpp       # Low-Level Port I/O Helper Header
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `libkern.hpp` | Deklarasi base class `OSObject` dengan `retain()` / `release()` atomic refcounting. |
| `io.hpp` | Inline macro/fungsi perakitan instruksi Port I/O (`inb`, `outb`, `inw`, `outw`). |
