# Spesifikasi Virtual File System (VFS) Nebula OS (BSD VFS Layer)

## 1. Gambaran Umum
Virtual File System (VFS) Nebula OS diimplementasikan pada BSD OS Services Layer (`include/bsd/vfs/` & `kernel/bsd/vfs/`) untuk menyediakan antarmuka terpadu operasi file bagi aplikasi Ring 3 maupun subsistem kernel Ring 0.

---

## 2. Struktur Abstraksi VNode (`vfs.hpp`)

Setiap file dan direktori diwakili oleh struktur `vnode_t`:

```cpp
struct vnode_t {
    char name[32];
    uint32_t type;       // VNODE_TYPE_FILE atau VNODE_TYPE_DIR
    uint32_t size;       // Ukuran file dalam bytes
    vfs_ops_t* ops;      // Pointer ke tabel operasi file konkrit
    void* internal_data; // Pointer internal ke data driver (Initrd RAM Disk)
};
```

---

## 3. Modul Initrd RAM Disk (`initrd.hpp` & `initrd.cpp`)

* Driver RAM Disk memuat file dari memori bootloader saat inisialisasi awal.
* Terpasang di `/initrd/` dan mendukung fungsi `open`, `read`, dan `readdir`.
