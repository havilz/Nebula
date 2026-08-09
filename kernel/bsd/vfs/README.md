# BSD Virtual File System Implementation

Folder `kernel/bsd/vfs/` berisi implementasi VFS Layer dan Initrd Driver.

## Source Tree

```text
kernel/bsd/vfs/
├── vfs.cpp     # Implementasi Abstraksi VFS & VNode Operations
└── initrd.cpp  # Implementasi Driver RAM Disk /initrd/
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `vfs.cpp` | Implementasi registrasi sistem berkas, pembuatan node, dan pembacaan direktori. |
| `initrd.cpp` | Implementasi ekstraksi file header dari RAM Disk multiboot module. |
