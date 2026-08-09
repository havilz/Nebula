# Libkern C++ Runtime Implementation

Folder `kernel/libkern/` berisi kode sumber implementasi utilitas C++ runtime kernel.

## Source Tree

```text
kernel/libkern/
└── libkern.cpp  # Implementasi OSObject Refcounting & Libkern Init
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `libkern.cpp` | Implementasi metode `retain()` / `release()` berbasis atomic refcount pada `OSObject`. |
