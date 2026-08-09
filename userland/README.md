# Userland Directory - Ring 3 Ecosystem

Folder `userland/` merupakan ruang ekosistem pengguna Ring 3 pada **Nebula OS** yang berisi Standard C Library (`libnebula`/`libc`) dan aplikasi-aplikasi pengguna (*Userland Applications*).

## Source Tree

```text
userland/
├── libc/       # Standard C Library (libnebula) & C Runtime crt0.asm
└── apps/       # Aplikasi Ring 3 (Process Init PID 1 & Nebula Shell App)
```

## Sub-Direktori

* **`libc/`**: Berisi header (`<stdio.h>`, `<stdlib.h>`, `<string.h>`, `<unistd.h>`) dan implementasi wrapper system call `INT 0x80`.
* **`apps/`**: Berisi kode sumber aplikasi native Ring 3 (`init` dan `shell`).
