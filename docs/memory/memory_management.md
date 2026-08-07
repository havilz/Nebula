# Spesifikasi Manajemen Memori Nebula OS

## 1. Gambaran Umum
Manajemen memori Nebula OS dibagi menjadi tiga tingkatan hirarki utama:
1. **Physical Memory Manager (PMM)**: Mengelola frame fisik memori RAM (4 KiB per frame) menggunakan struktur data Bitmap.
2. **Virtual Memory Manager (VMM)**: Mengelola pemetaan alamat virtual ke frame fisik menggunakan struktur Paging 4-Tingkat x86_64 (PML4, PDPT, PD, PT).
3. **Kernel Heap Allocator**: Menyediakan fungsi alokasi memori dinamis `kmalloc()` dan `kfree()`, serta overloading operator global C++ `new` dan `delete`.

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

## 3. Virtual Memory Manager (VMM) & Paging x86_64

### 3.1 Paging 4-Tingkat (PML4)
Dalam mode 64-bit Long Mode, translasi alamat virtual dilakukan melalui 4 level tabel:

```
Virtual Address [47:0]
├── Bits [47:39] -> PML4 Index   (Page Map Level 4)
├── Bits [38:30] -> PDPT Index   (Page Directory Pointer Table)
├── Bits [29:21] -> PD Index     (Page Directory)
├── Bits [20:12] -> PT Index     (Page Table)
└── Bits [11:0]  -> Physical Page Offset
```

### 3.2 Pemetaan Higher-Half Kernel
* **Alamat Virtual Higher-Half**: `0xFFFF800000000000` s/d `0xFFFFFFFFFFFFFFFF`.
* Seluruh memori RAM fisik dipetakan secara langsung (*Direct Physical Mapping*) di `0xFFFF800000000000` untuk memudahkan kernel mengakses sembarang alamat fisik RAM.

---

## 4. Kernel Heap Allocator

### 4.1 Desain Allocator
Kernel Heap mengelola blok memori dinamis di atas alamat virtual kernel yang dialokasikan oleh VMM:
* Menggunakan gabungan algoritma **Free List / Slab Allocator** untuk alokasi memori berukuran kecil.
* Alokasi besar dialokasikan langsung dalam kelipatan 4 KiB halaman VMM.

### 4.2 Dukungan C++ Dynamic Memory
Dua operator bawaan C++ berikut di-overload untuk memanggil `kmalloc` dan `kfree`:

```cpp
void* operator new(size_t size) {
    return kmalloc(size);
}

void operator delete(void* ptr) noexcept {
    kfree(ptr);
}
```
