# RULES.md - Panduan & Aturan Penulisan Kode Nebula OS

Dokumen ini berisi aturan wajib (Strict Coding Standards) dalam pengembangan **Nebula OS**. Semua kontributor dan modul wajib mematuhi aturan ini demi menjaga keamanan memori, kestabilan kernel, dan performa tinggi.

---

## 1. Aturan Dasar C++ Bare-Metal (Freestanding)

1. **Dilarang Menggunakan C++ Standard Library Default**:
   * Header seperti `<iostream>`, `<vector>`, `<string>`, `<thread>`, `<mutex>` DILARANG diimpor di ruang kernel (`kernel/`).
   * Hanya header freestanding bawaan compiler yang boleh digunakan: `<stdint.h>`, `<stddef.h>`, `<stdbool.h>`, `<stdarg.h>`, `<limits.h>`.

2. **Bahasa Komentar Kode (English Only)**:
   * Seluruh komentar di dalam kode (*inline comments*, *header docstrings*, & *assembly comments*) **WAJIB menggunakan Bahasa Inggris**.

3. **Non-Exceptions & Non-RTTI**:
   * Kode dikompilasi dengan `-fno-exceptions -fno-rtti`.
   * Dilarang menggunakan `try`, `catch`, `throw`, `dynamic_cast`, atau `typeid`.
   * Gunakan pengembalian status kesalahan berbasis `Result<T, ErrorCode>` atau kode `int` negatif.

4. **Tanpa Variabel Global yang Mengalokasi Memori Sebelum Kernel Inisialisasi**:
   * Variabel global dengan konstruktor kompleks tidak boleh digunakan karena C++ runtime initialization (`__do_global_ctors`) belum berjalan saat bootloader menyerahkan kontrol.

---

## 2. Manajemen Memori & Pointer

1. **Larangan Memory Leak**:
   * Setiap alokasi memori melalui `kmalloc()` atau `operator new` wajib memiliki pasangan `kfree()` atau `delete`.
2. **Prioritaskan RAII (Resource Acquisition Is Initialization)**:
   * Gunakan pembungkus smart pointer custom (`UniquePtr<T>`, `SharedPtr<T>`) untuk mengelola umur objek di dalam kernel.
3. **Validasi Pointer Sebelum Dereference**:
   * Selalu lakukan pemeriksaan `if (ptr == nullptr)` sebelum mengakses data di balik pointer untuk mencegah Kernel Panic / Page Fault.

---

## 3. Konvensi Penamaan (Naming Conventions)

| Elemen Kode | Konvensi | Contoh |
| :--- | :--- | :--- |
| **Class / Struct / Enum** | `PascalCase` | `PhysicalMemoryManager`, `ProcessControlBlock` |
| **Fungsi / Method** | `snake_case` atau `camelCase` | `initialize_paging()`, `allocateFrame()` |
| **Variabel / Parameter** | `snake_case` | `page_table_index`, `frame_buffer` |
| **Variabel Anggota Class** | Prefix `m_` + `camelCase` | `m_processId`, `m_pageDirectory` |
| **Konstanta / Enum Value** | `ALL_CAPS_SNAKE` | `MAX_PROCESSES`, `PAGE_SIZE_4K` |
| **Macro Assembly / C** | `ALL_CAPS_SNAKE` | `MULTIBOOT_HEADER_MAGIC` |
| **File Header / Source** | `snake_case` (`.hpp` / `.cpp`) | `virtual_memory.hpp`, `scheduler.cpp` |

---

## 4. Keamanan Interupsi & Ring 0

1. **Operasi Atomic & Critical Section**:
   * Saat mengubah struktur data kritis kernel (seperti antrean scheduler atau daftar memori bebas), interupsi hardware wajib dimatikan sementara (`cli`) dan diaktifkan kembali (`sti`), atau menggunakan Spinlock.
2. **ISR (Interrupt Service Routine) Harus Singkat**:
   * Kode di dalam penangan interupsi (ISR) harus dieksekusi secepat mungkin. Tugas berat harus didelegasikan ke Deferred Procedure Call (DPC) / Tasklet.

---

## 5. Kebijakan Kompilasi & Warning

* **Zero Warnings Policy**: Kode wajib bersih tanpa warning saat dikompilasi dengan `-Wall -Wextra -Werror`.
* **Explicit Cast**: Hindari implicit casting. Gunakan `static_cast`, `reinterpret_cast`, atau `const_cast` secara bijak dan eksplisit.
