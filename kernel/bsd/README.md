# BSD OS Services Implementation

Folder `kernel/bsd/` berisi kode sumber implementasi POSIX OS Services.

## Source Tree

```text
kernel/bsd/
├── vfs/      # Implementasi Virtual File System & Initrd RAM Disk
└── sys/      # Implementasi System Call Dispatcher Vector 128
```

## Sub-Direktori

* **`vfs/`**: Implementasi struktur vnode dan pembacaan berkas pada RAM Disk.
* **`sys/`**: Implementasi penerima System Call Ring 3 (`INT 0x80`).
