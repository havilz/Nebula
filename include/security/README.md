# Security & MAC Framework Headers

Folder `include/security/` berisi deklarasi antarmuka keamanan dan kontrol akses terproteksi (*Mandatory Access Control*).

## Source Tree

```text
include/security/
└── security.hpp  # MAC Security & User Credentials Header
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `security.hpp` | Deklarasi struktur kredensial pengguna (`ucred_t`) dan fungsi verifikasi hak akses (`check_permission`). |
