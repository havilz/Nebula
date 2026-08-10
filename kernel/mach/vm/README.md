# Mach Memory Management Implementation

Folder `kernel/mach/vm/` berisi kode sumber pengelola memori fisik, virtual, dan heap.

## Source Tree

```text
kernel/mach/vm/
├── pmm.cpp     # Implementasi Bitmap Physical Memory Manager
├── vmm.cpp     # Implementasi Virtual Memory Manager Paging
├── heap.cpp    # Implementasi Kernel Heap Allocator (kmalloc/kfree)
└── kheap.cpp   # Implementasi Overloading Global Operator C++ new & delete
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `pmm.cpp` | Pelacakan dan alokasi frame memori 4 KiB menggunakan struktur bitmap. |
| `vmm.cpp` | Pemetaan halaman memori virtual ke frame fisik (termasuk LFB VRAM `0xFD000000`). |
| `heap.cpp` | Manajemen alokasi memori dinamis kernel `kmalloc` dan dealokasi `kfree`. |
| `kheap.cpp` | Overloading operator global C++ `new`, `new[]`, `delete`, dan `delete[]`. |
