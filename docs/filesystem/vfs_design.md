# Spesifikasi Desain Virtual File System (VFS)

## 1. Arsitektur VFS
Virtual File System (VFS) mengabstraksikan berbagai jenis media penyimpanan dan sistem berkas di bawah satu antarmuka terpadu.

```
                  +--------------------------+
                  | System Calls (Ring 3)    |
                  | open, read, write, close |
                  +--------------------------+
                               |
                               v
                  +--------------------------+
                  | VFS Abstraction Layer    |
                  +--------------------------+
                               |
      +------------------------+------------------------+
      |                        |                        |
      v                        v                        v
+-----------+            +-----------+            +-----------+
| Initrd FS |            | RAMFS     |            | FAT32 FS  |
+-----------+            +-----------+            +-----------+
```

---

## 2. Abstraksi Utama

1. **`VNode` (Virtual Node)**:
   Mewakili sebuah file atau direktori dalam memori RAM kernel. Menyimpan pointer ke operasi file (`read`, `write`, `readdir`, `finddir`).

2. **`MountPoint`**:
   Menghubungkan root dari sebuah sistem berkas spesifik ke suatu jalur direktori dalam pohon VFS (misalnya memetakan RAMFS ke `/`).

3. **`FileDescriptor`**:
   Struktur data per-proses yang mencatat posisi offset pembacaan/penulisan file saat ini (*file offset*) dan mode akses (ReadOnly, WriteOnly, ReadWrite).
