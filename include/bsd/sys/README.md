# BSD System Call & Executable Headers

Folder `include/bsd/sys/` berisi deklarasi header untuk antarmuka System Call, ELF Executable Loader, dan Native App Bundle (.app/.neb).

## Source Tree

```text
include/bsd/sys/
├── syscall.hpp  # System Call Gate Handler (INT 0x80) Header
├── elf.hpp      # ELF Executable Header & Loader Declaration
├── bundle.hpp   # Native App Bundle (.app/.neb) Package Format Header
└── README.md    # System Call Subsystem Header Documentation
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `syscall.hpp` | Deklarasi gate handler Vector 128 (`INT 0x80`) dan daftar nomor panggilan sistem POSIX (`SYS_READ`, `SYS_WRITE`, `SYS_EXEC`, `SYS_MALLOC`). |
| `elf.hpp` | Deklarasi struktur `Elf32_Ehdr`, `Elf32_Phdr`, dan kelas `ELFLoader` pemroses biner executable Ring 3. |
| `bundle.hpp` | Deklarasi struktur metadata bundel aplikasi native `.app` / `.neb` (`app_bundle_t`). |
