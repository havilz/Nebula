# BSD System Call & Executable Implementation

Folder `kernel/bsd/sys/` berisi kode sumber implementasi C++ penangan System Call dan pemuat biner ELF.

## Source Tree

```text
kernel/bsd/sys/
├── syscall.cpp  # Implementasi System Call Dispatcher Vector 128 (INT 0x80)
├── elf.cpp      # Implementasi ELF Executable Loader & Segment Allocation
└── README.md    # System Call Subsystem Implementation Documentation
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `syscall.cpp` | Pendaftaran penangan interupsi Vector 128 (`INT 0x80`) dan pemrosesan panggilan sistem POSIX. |
| `elf.cpp` | Pengesahan header ELF32/ELF64 (`0x7F 'E' 'L' 'F'`), penguraian segment `PT_LOAD`, dan alokasi stack 16 KiB. |
