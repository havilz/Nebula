# BSD System Call Implementation

Folder `kernel/bsd/sys/` berisi implementasi penangan System Call.

## Source Tree

```text
kernel/bsd/sys/
└── syscall.cpp  # Implementasi System Call Gate Handler Vector 128
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `syscall.cpp` | Pendaftaran penangan interupsi Vector 128 (`INT 0x80`) dan pemrosesan `SYS_WRITE`. |
