# BSD Virtual File System Headers

Folder `include/bsd/vfs/` berisi deklarasi header untuk VFS Abstraction Layer dan Initrd.

## Source Tree

```text
include/bsd/vfs/
├── vfs.hpp     # Virtual File System & VNode Abstraction Header
└── initrd.hpp  # Initrd RAM Disk Driver Header
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `vfs.hpp` | Deklarasi struktur `vnode_t`, operasi file (`open`, `read`, `write`), dan VFS Mount Point. |
| `initrd.hpp` | Deklarasi driver RAM Disk pendukung pemuatan file awal saat boot. |
