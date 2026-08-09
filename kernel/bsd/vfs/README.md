# BSD Virtual File System Implementation

Folder `kernel/bsd/vfs/` berisi implementasi VFS Layer, Initrd RAM Disk Driver, dan FAT32 Filesystem Driver.

## Source Tree

```text
kernel/bsd/vfs/
├── vfs.cpp     # Implementasi Abstraksi VFS & VNode Operations
├── initrd.cpp  # Implementasi Driver RAM Disk /initrd/
├── fat32.cpp   # Implementasi Driver System Berkas Permanen /fat32/
└── README.md   # VFS Subsystem Implementation Documentation
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `vfs.cpp` | Implementasi registrasi sistem berkas, pembuatan node, dan pembacaan direktori. |
| `initrd.cpp` | Implementasi ekstraksi file header dari RAM Disk multiboot module. |
| `fat32.cpp` | Implementasi BPB Bios Parameter Block parsing, kalkulasi cluster LBA, dan pembacaan/penulisan file FAT32. |
