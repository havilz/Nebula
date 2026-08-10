# Standard C Library (libnebula / libc)

Folder `userland/libc/` berisi header dan kode sumber implementasi Standard C Library khusus untuk aplikasi Ring 3 Nebula OS.

## Source Tree

```text
userland/libc/
├── include/
│   ├── stdio.h        # Standard IO Header (printf, puts, putchar)
│   ├── stdlib.h       # Standard Utility Header (exit, atoi, malloc, free)
│   ├── string.h       # String Operations Header (strlen, strcpy, strcmp, memset)
│   ├── unistd.h       # POSIX Syscall Wrappers Header (write, read, fork, exec)
│   └── sys/
│       └── types.h    # Standard POSIX Types Header
└── src/
    ├── crt0.asm       # C Runtime Entry Point _start Stub
    ├── stdio.c        # Implementasi printf & I/O
    ├── stdlib.c       # Implementasi exit, atoi, memory allocation
    ├── string.c       # Implementasi string manipulation
    └── unistd.c       # Implementasi wrappers system call INT 0x80
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `crt0.asm` | Assembly stub `_start` yang menyiapkan stack dan mengeksekusi `main(argc, argv)` lalu `exit()`. |
| `stdio.c` | Perenderan string terformat `printf`, `putchar`, dan `puts` melalui system call `SYS_WRITE`. |
| `stdlib.c` | Fungsi utilitas standar `exit`, `atoi`, `abs`, `malloc`, dan `free`. |
| `string.c` | Manipulasi string C (`strlen`, `strcpy`, `strcmp`, `memset`, `memcpy`). |
| `unistd.c` | Pembungkus instruksi assembly `int $0x80` untuk `write`, `read`, `close`, `fork`, `execve`, dan `yield`. |
