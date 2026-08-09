# BSD Virtual File System Headers

Folder `include/bsd/vfs/` berisi deklarasi header untuk VFS Abstraction Layer, Initrd RAM Disk, dan FAT32 Filesystem Driver.

## Source Tree

```text
include/bsd/vfs/
├── vfs.hpp     # Virtual File System & VNode Abstraction Header
├── initrd.hpp  # Initrd RAM Disk Driver Header
├── fat32.hpp   # BSD FAT32 Filesystem Driver Header
└── README.md   # VFS Subsystem Header Documentation
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `vfs.hpp` | Deklarasi struktur `vnode_t`, operasi file (`read`, `write`, `finddir`), dan VFS Mount Point. |
| `initrd.hpp` | Deklarasi driver RAM Disk pendukung pemuatan file awal saat boot. |
| `fat32.hpp` | Deklarasi kelas `FAT32`, struktur `fat32_bpb_t`, dan `fat32_dir_entry_t` untuk meng-mount sistem berkas permanen FAT32. |
