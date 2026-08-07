# Nebula OS - Gambaran Umum Arsitektur Sistem

## 1. Visi & Tujuan Utama
Nebula OS adalah sistem operasi 64-bit (x86_64) modular yang dibangun dari nol menggunakan C++ dan Assembly. Sistem ini dirancang untuk menggabungkan pemisahan kernel micro/hybrid yang bersih dengan performa tinggi, keamanan tipe data (*type safety*), abstraksi konkurensi modern, hingga antarmuka grafis (GUI Window Manager).

---

## 2. Paradigma Arsitektur
Nebula OS mengadopsi desain **Modular Hybrid Kernel**:

* **Kernel Utama Terproteksi (Ring 0)**:
  * Abstraksi Arsitektur CPU (GDT, IDT, Paging, Context Switching)
  * Manajemen Memori Fisik & Virtual (Bitmap Allocator, Page Tables PML4)
  * Scheduler Proses & Thread (Multitasking Preemptif, MLFQ / Round-Robin)
  * Komunikasi Antar-Proses / IPC (Channel, Shared Memory, Pipe)
  * Virtual File System (Abstraksi VFS)
  * Driver Hardware Dasar (VGA/VBE Framebuffer, Keyboard PS/2, PIT/APIC Timer)

* **Area Pengguna / Userland (Ring 3)**:
  * Library Standar C khusus (`libc`) & Runtime C++
  * Inisialisasi Sistem (Proses `init`)
  * Command Shell (`shell`) & Utilitas CLI
  * Compositor Antarmuka Grafis (Lingkungan Desktop/GUI)

---

## 3. Tata Letak Sistem & Peta Memori x86_64

```
+-----------------------------------+ 0xFFFFFFFFFFFFFFFF (Batas Atas Virtual Address Space)
|                                   |
|       Higher-Half Kernel Space    | (Pemetaan langsung memori fisik & heap kernel)
|         0xFFFF800000000000        |
+-----------------------------------+ 0xFFFF800000000000
|       Unmapped Canonical Hole     | (Area tak terpetakan)
+-----------------------------------+ 0x00007FFFFFFFFFFF
|                                   |
|         Userland Space            | (Kode Proses, Stack, & Heap Aplikasi Ring 3)
|                                   |
+-----------------------------------+ 0x0000000000000000
```

---

## 4. Rincian Subsistem Utama

### 4.1 Lapisan Arsitektur (`kernel/arch/x86_64/`)
Membungkus semua instruksi assembly spesifik x86_64, manajemen register CPU, Global Descriptor Table (GDT), Interrupt Descriptor Table (IDT), register kontrol paging PML4, dan stub context switching.

### 4.2 Subsistem Core (`kernel/core/`)
Berisi orkestrasi startup kernel (`kernel.cpp`, `init.cpp`), penanganan kondisi darurat (`panic.cpp`), sistem logging terformat (`log.cpp`), dan pelacakan status global CPU.

### 4.3 Manajemen Memori (`kernel/memory/`)
* **Physical Memory Manager (PMM)**: Allocator berbasis bitmap untuk melacak frame fisik memori berukuran 4KiB.
* **Virtual Memory Manager (VMM)**: Pengelola paging 4-tingkat PML4 yang memetakan page virtual ke frame fisik dengan atribut keamanan (Read/Write/User).
* **Heap Manager**: Implementasi `kmalloc`/`kfree` kernel dan overloading operator global C++ `new`/`delete`.

### 4.4 Proses & Scheduler (`kernel/process/`, `kernel/scheduler/`)
Mengelola Process Control Block (PCB), Thread Control Block (TCB), hirarki proses, alokasi PID unik, context switching preemptif berbasis interupsi timer, dan kebijakan penjadwalan (Round-Robin, MLFQ).

### 4.5 Virtual Filesystem (`kernel/filesystem/vfs/`)
Mengabstraksikan operasi file (`open`, `read`, `write`, `close`, `readdir`) di atas berbagai jenis sistem berkas (Initrd, RAMFS, FAT32/Ext2).

### 4.6 Driver Perangkat (`kernel/drivers/`)
Antarmuka driver modular untuk Konsol Layar, Keyboard, Serial Port (COM1 untuk debugging), Media Penyimpanan, dan Timer.

### 4.7 Lapisan System Call (`kernel/syscall/`)
Pengendali `SYSCALL`/`SYSRET` x86_64 berkecepatan tinggi yang memetakan permintaan aplikasi Ring 3 Userland ke fungsi internal Ring 0 Kernel.

---

## 5. Standar Kode & Panduan C++
* **Freestanding C++**: Tanpa C++ Standard Library bawaan OS (`<iostream>`, `<vector>`, `<string>` dilarang di Ring 0 sampai diimplementasikan di `libc` internal kita).
* **Tanpa Exception & RTTI**: Dikompilasi dengan `-fno-exceptions -fno-rtti` demi determinisme dan performa maksimal.
* **Manajemen Memori Eksplisit**: Semua alokasi memori wajib dibebaskan secara eksplisit atau dikelola menggunakan abstraksi RAII custom (`UniquePtr`, `SharedPtr`).
