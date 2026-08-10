# Userland Applications Directory

Folder `userland/apps/` berisi kode sumber aplikasi-aplikasi native yang berjalan di mode Ring 3 Userland Nebula OS.

## Source Tree

```text
userland/apps/
├── init/
│   └── main.c     # Process Init (PID 1) Application Source
└── shell/
    └── main.c     # Nebula Interactive Shell Application Source
```

## Daftar Aplikasi

| Aplikasi | Deskripsi & Fungsi |
| :--- | :--- |
| **`init`** | Proses pertama (PID 1) yang di-fork oleh kernel untuk menginisialisasi lingkungan userland dan memuat shell. |
| **`shell`** | Aplikasi antarmuka perintah interaktif pengguna Ring 3. |
