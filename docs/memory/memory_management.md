# Spesifikasi Manajemen Memori Nebula OS (Mach VM Layer)

## 1. Gambaran Umum
Manajemen memori Nebula OS diletakkan pada Mach Microkernel Core Layer (`include/mach/vm/` & `kernel/mach/vm/`) dan dibagi menjadi tiga tingkatan hirarki utama:
1. **Physical Memory Manager (PMM)** (`pmm.hpp` & `pmm.cpp`): Mengelola frame fisik memori RAM (4 KiB per frame) menggunakan struktur data Bitmap.
2. **Virtual Memory Manager (VMM)** (`vmm.hpp` & `vmm.cpp`): Mengelola pemetaan alamat virtual ke frame fisik menggunakan struktur Paging x86_64.
3. **Kernel Heap Allocator** (`heap.hpp`, `heap.cpp`, `kheap.cpp`): Menyediakan fungsi alokasi memori dinamis `kmalloc()` dan `kfree()`, serta overloading operator global C++ `new` dan `delete`.

---

## 2. Physical Memory Manager (PMM)

### 2.1 Mekanisme Bitmap
* PMM membagi seluruh memori RAM fisik yang tersedia menjadi blok-blok berukuran 4 KiB (4096 byte).
* Setiap bit pada bitmap mewakili 1 frame fisik:
  * Bit `0`: Frame kosong/bebas (dapat dialokasikan).
  * Bit `1`: Frame terpakai (dialokasikan untuk kernel, page table, atau stack).

### 2.2 Alokasi & Dealokasi
* `pmm_allocate_frame()`: Mencari bit `0` pertama pada bitmap, mengubahnya menjadi `1`, lalu mengembalikan alamat fisik frame.
* `pmm_free_frame(uintptr_t frame_addr)`: Mengubah bit pada indeks frame yang bersangkutan kembali menjadi `0`.

---

## 3. Virtual Memory Manager (VMM) & Paging

### 3.1 Paging & Identity Mapping
Dalam mode x86_64 Paging, translasi alamat virtual dilakukan melalui tabel paging:

```text
Virtual Address [47:0]
├── Bits [47:39] -> PML4 Index   (Page Map Level 4)
├── Bits [38:30] -> PDPT Index   (Page Directory Pointer Table)
├── Bits [29:21] -> PD Index     (Page Directory)
├── Bits [20:12] -> PT Index     (Page Table)
└── Bits [11:0]  -> Physical Page Offset
```

### 3.2 Pemetaan Higher-Half Kernel & Video RAM
* **Alamat Virtual Higher-Half**: `0xFFFF800000000000` s/d `0xFFFFFFFFFFFFFFFF`.
* **Hardware Framebuffer VRAM**: Memetakan alamat fisik Video RAM `0xFD000000` dan `0xE0000000` di VMM page tables agar penulisan piksel Bochs BGA tidak pernah memicu Page Fault (`#PF`).

---

## 4. Kernel Heap Allocator

### 4.1 Desain Allocator
Kernel Heap mengelola pool memori dinamis (`0x00C00000`) di atas alamat virtual kernel yang dialokasikan oleh VMM:
* Menggunakan alokator terstruktur untuk alokasi memori berukuran kecil hingga besar.
* Alokasi besar dialokasikan langsung dalam kelipatan 4 KiB halaman VMM.

### 4.2 Dukungan C++ Dynamic Memory
Dua operator bawaan C++ berikut di-overload pada `kernel/mach/vm/kheap.cpp`:

```cpp
void* operator new(size_t size) {
    return kmalloc(size);
}

void operator delete(void* ptr) noexcept {
    kfree(ptr);
}
```
