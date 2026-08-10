# Kernel Core Entry Point

Folder `kernel/core/` berisi kode sumber utama entry point kernel **Nebula OS**.

## Source Tree

```text
kernel/core/
└── kernel.cpp  # Main Entry Point kernel_main() & Subsystems Orchestrator
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `kernel.cpp` | Titik masuk utama `kernel_main()`, unity include seluruh file `.cpp` subsistem, serta pengorkestrasi inisialisasi XNU modules. |
