# BSD System Call Headers

Folder `include/bsd/sys/` berisi deklarasi header antarmuka System Call.

## Source Tree

```text
include/bsd/sys/
└── syscall.hpp  # System Call Dispatcher (INT 0x80) Header
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `syscall.hpp` | Deklarasi IDT Vector 128 gate handler dan tabel penanganan nomor system call (`SYS_WRITE`, dll). |
