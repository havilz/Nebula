# BSD POSIX OS Services Headers

Folder `include/bsd/` berisi deklarasi header untuk layanan sistem operasi berstandar POSIX BSD (Virtual File System & System Call).

## Source Tree

```text
include/bsd/
├── vfs/      # Virtual File System & Initrd Headers
└── sys/      # System Call Interface Headers
```

## Sub-Direktori

* **`vfs/`**: Mengabstraksikan struktur vnode dan sistem berkas RAM disk `/initrd/`.
* **`sys/`**: Menangani pendaftaran dan dispatcher system call Vector 128 (`INT 0x80`).
