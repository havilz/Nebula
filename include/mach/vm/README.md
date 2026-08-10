# Mach Virtual Memory Headers

Folder `include/mach/vm/` berisi deklarasi header untuk pengelolaan memori fisik, virtual, dan heap kernel.

## Source Tree

```text
include/mach/vm/
├── pmm.hpp     # Physical Memory Manager (Bitmap Allocator) Header
├── vmm.hpp     # Virtual Memory Manager (Paging Manager) Header
└── heap.hpp    # Kernel Heap Allocator Header
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `pmm.hpp` | Deklarasi PMM berbasis bitmap untuk alokasi/dealokasi frame memori 4 KiB. |
| `vmm.hpp` | Deklarasi VMM untuk pemetaan alamat virtual, identity mapping, dan proteksi page. |
| `heap.hpp` | Deklarasi Kernel Heap Manager (`kmalloc`/`kfree`) dan pool memori dinamis. |
