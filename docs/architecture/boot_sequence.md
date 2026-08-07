# Nebula OS - Spesifikasi Alur Booting (Boot Sequence)

## 1. Ringkasan Alur Eksekusi Booting

Alur booting Nebula OS memindahkan CPU dari status awal menyala (atau saat emulator dijalankan) menuju lingkungan eksekusi C++ 64-bit Long Mode yang utuh.

```
+-------------------------------------------------------+
|  1. Firmware / BIOS / Multiboot Bootloader            |
|     Memuat binary ELF bootable ke RAM di 1MB (0x100000) |
+-------------------------------------------------------+
                           |
                           v
+-------------------------------------------------------+
|  2. Entry Point Assembly (`boot/x86_64/boot.asm`)     |
|     - Verifikasi Magic Number Multiboot               |
|     - Menyiapkan Stack Pointer 32-bit                 |
|     - Memeriksa Dukungan CPUID & 64-bit Long Mode     |
+-------------------------------------------------------+
                           |
                           v
+-------------------------------------------------------+
|  3. Paging & Transisi Long Mode (`entry.asm`)         |
|     - Pemetaan Identity 2MB Pertama + Higher-Half     |
|     - Menyiapkan Struktur Page Table PML4, PDPT, PD   |
|     - Aktifkan PAE (Physical Address Extension) CR4   |
|     - Aktifkan Long Mode pada EFER MSR                |
|     - Aktifkan Paging pada CR0                        |
|     - Memuat GDT (Global Descriptor Table) 64-bit     |
|     - Far Jump ke Subspace Kode 64-bit                |
+-------------------------------------------------------+
                           |
                           v
+-------------------------------------------------------+
|  4. Main Kernel C++ (`kernel/core/kernel.cpp`)        |
|     - Inisialisasi Console Logging VGA/VBE            |
|     - Inisialisasi GDT & IDT Vector Interupsi Kernel  |
|     - Inisialisasi Engine Memori PMM & VMM Paging     |
|     - Inisialisasi Heap Manager Kernel (`kmalloc`)    |
|     - Memuat Driver Hardware (Keyboard, Timer, Serial)|
|     - Inisialisasi Scheduler & Membuat Proses 0 (Init)|
|     - berpindah ke Userland Ring 3 Shell / GUI        |
+-------------------------------------------------------+
```

---

## 2. Rincian Fase Booting

### Fase 1: Serah Terima dari Bootloader
Bootloader standar Multiboot (GRUB atau QEMU `-kernel`) memuat file `nebula.bin` ke memori fisik pada alamat `0x100000` (1 MiB).
Bootloader mengirimkan parameter:
* Register `EAX`: Magic number `0x2BADB002` (Multiboot 1) atau `0x36D32890` (Multiboot 2).
* Register `EBX`: Alamat pointer ke struktur Informasi Multiboot di RAM.

### Fase 2: Pengujian Kemampuan CPU
Sebelum mencoba berpindah ke mode 64-bit, `boot.asm` melakukan serangkaian pemeriksaan keamanan:
1. **Pemeriksaan Multiboot**: Memastikan register `EAX` berisi magic number yang valid.
2. **Pemeriksaan Dukungan CPUID**: Mengubah flag ID pada register EFLAGS untuk memastikan CPU mendukung instruksi CPUID.
3. **Pemeriksaan Long Mode**: Memanggil CPUID fungsi extended `0x80000001` bit 29 untuk memastikan prosesor mendukung mode 64-bit.

### Fase 3: Penyiapan Table Page (Identity & Higher-Half)
Untuk masuk ke 64-bit Long Mode, paging **WAJIB** diaktifkan. File `entry.asm` membangun hirarki page table 4 tingkat:
* **PML4** (Page Map Level 4): Entry pertama memetakan alamat rendah secara identik (`0x0 - 0x200000`), entry ke-511 memetakan alamat Higher-Half Kernel (`0xFFFF800000000000`).
* **PDPT** (Page Directory Pointer Table)
* **PD** (Page Directory): Menggunakan 2MB Huge Pages (bit `PS` aktif) untuk efisiensi pemetaan awal.

### Fase 4: Aktivasi Long Mode 64-bit
1. Set bit 5 pada CR4 (PAE - Physical Address Extension).
2. Baca MSR IA32_EFER (`0xC0000080`), set bit 8 (LME - Long Mode Enable).
3. Set bit 31 pada CR0 (PG - Paging Enable).
4. Muat GDT 64-bit yang berisi Segment Selector Kode (`0x08`) dan Data (`0x10`).
5. Lakukan *Far Jump* `jmp 0x08:kernel_entry64` untuk mengosongkan *pipeline* instruksi 32-bit dan secara resmi masuk ke **64-bit Long Mode**.

### Fase 5: Eksekusi Lingkungan C++ Kernel
Setelah berada di mode 64-bit:
* Register `RAX`, `RBX`, `RCX`, `RDX`, `RSI`, `RDI`, `RBP`, dan `R8`-`R15` berukuran 64-bit siap digunakan.
* Menyiapkan lokasi Stack 64-bit di memori BSS.
* Memanggil fungsi `kernel_main()` di dalam file `kernel/core/kernel.cpp`.
