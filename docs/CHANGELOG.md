# CHANGELOG & DOKUMENTASI TEKNIS - NEBULA OS

Dokumen ini mencatat seluruh riwayat perubahan, keputusan arsitektur teknis, diagnosa masalah, dan solusi yang diterapkan pada proyek **Nebula OS**. Dokumen ini diperbarui secara berkala setiap kali suatu Fase / Task selesai diimplementasikan.

---

## [Task 1] - Barebones Kernel & Multiboot 1 Verification

### Ringkasan Tujuan
Membuat kernel 32-bit x86 paling mendasar (*barebones*) yang mematuhi **Spesifikasi Multiboot 1**, mampu di-boot secara langsung oleh QEMU / Bootloader, serta menyediakan driver layar konsol mode teks VGA 80x25 dan driver logging serial UART COM1.

---

### Perubahan & Komponen Utama yang Dibuat

#### 1. Assembly Entry Point (`boot/x86_64/boot.asm`)
- **Header Multiboot 1**: Mengonfigurasi Magic Number (`0x1BADB002`), Flag (`0x3` = `ALIGN | MEMINFO`), dan Checksum matematika yang valid.
- **Immediate `cli`**: Menambahkan instruksi `cli` (*Clear Interrupts*) sebagai instruksi paling pertama pada `_start` untuk menjamin tidak ada interrupt hardware (IRQ0 Timer) yang masuk sebelum IDT dikonfigurasi.
- **System V ABI Stack Alignment**:
  - Alokasi memori stack sebesar 16 KiB pada section `.bss`.
  - Mengonfigurasi alignment stack 16-byte (`and esp, -16` dan `sub esp, 12`) untuk mematuhi konvensi pemanggilan fungsi System V i386 ABI C/C++.
- **Panggilan Kernel**: Memanggil `kernel_main()` dan menyediakan *infinite halt loop* (`.hang: cli; hlt; jmp .hang`).

#### 2. Linker Script (`boot/linker.ld`)
- Menentukan `ENTRY(_start)` sebagai entry point utama.
- Menetapkan base virtual/physical address memori kernel pada **`0x100000` (1 MiB)**.
- Menggabungkan section `.multiboot`, `.text`, `.rodata`, `.rdata`, `.data`, `.bss`, dan `COMMON` dengan dukungan flag `--omagic` untuk memastikan kernel dimuat sebagai **1 segmen ELF terpadu** tanpa padding 4KB yang merusak offset.

#### 3. Driver Konsol Mode Teks VGA (`include/kernel/drivers/console.hpp` & `kernel/drivers/console/vga.cpp`)
- **Video Buffer RAM**: Mengakses langsung alamat fisik `0xB8000` (ukuran 80x25 karakter).
- **Fungsi Utama**:
  - `VGAConsole::clear(bg)`: Membersihkan seluruh 2000 cell VGA dengan warna latar belakang (*background*) tertentu.
  - `VGAConsole::write_at(x, y, str, fg, bg)`: Menuliskan string ke posisi koordinat `(x, y)` tertentu dengan kombinasi warna teks (*foreground*) dan latar belakang.
- **Palet Warna**: Mendefinisikan enum `VGAColor` (16 warna standar VGA dari `COLOR_BLACK` hingga `COLOR_WHITE`).

#### 4. Driver Serial Logging UART 16550 (`include/kernel/drivers/serial.hpp` & `kernel/drivers/serial/serial.cpp`)
- **Port I/O**: Mengontrol port I/O serial COM1 (`0x3F8`).
- **LSR Status Polling**: Memeriksa bit 5 (*Transmit Holding Register Empty*) pada Line Status Register (`0x3FD`) sebelum mengirimkan byte karakter ke `0x3F8` untuk mencegah pembekuan (*stall*) atau karakter yang terbuang.
- **Fungsi Utama**: `Serial::init()`, `Serial::write_char()`, dan `Serial::write_string()`.

#### 5. Kernel Entry Point & Logic (`kernel/core/kernel.cpp`)
- Menginisialisasi driver serial `Serial::init()`.
- Mengirimkan jejak log status eksekusi real-time (*step-by-step logging*) ke port serial COM1.
- Membersihkan layar VGA dengan warna latar belakang biru (`COLOR_BLUE`).
- Menampilkan spanduk verifikasi Fase 1 dengan warna cyan, putih, hijau, dan kuning.
- Memasuki loop `while(true) { asm volatile ("hlt"); }` untuk menghentikan CPU secara aman.

#### 6. Build System & Toolchain (`Makefile`)
- Menggunakan toolchain **MSYS2 UCRT64** di Windows (`g++`, `nasm`, `ld.lld`, `objcopy`, `qemu-system-i386`).
- **Flag Pelindung Freestanding**:
  - `-ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -fno-leading-underscore`
  - `-fno-builtin -fno-tree-loop-distribute-patterns` (mencegah kompilator secara otomatis membuat panggilan ke `memset`/`memcpy` standar C library yang belum ada di kernel).
  - `-mno-sse -mno-sse2 -mno-mmx -mno-80387` (mematikan instruksi vektor SSE/FPU karena register kontrol CR4.OSFXSR belum diaktifkan).
- **Dukungan Dual Build Target**:
  - `mingw32-make run`: Membangun target release (`build/nebula.elf`) dan menjalankannya di QEMU GUI.
  - `mingw32-make debug`: Membangun target debug dengan simbol `-g -O0` (`build_debug/nebula.elf`).
  - `mingw32-make qemu-debug`: Membuka GDB Server QEMU di port 1234 dengan pause awal (`-s -S`).

#### 7. Integrasi VS Code Debugging (`.vscode/tasks.json` & `.vscode/launch.json`)
- Konfigurasi task build otomatis di VS Code dengan variabel lingkungan PATH MSYS2 UCRT64.
- Integrasi GDB 17 (`D:/msys64/ucrt64/bin/gdb.exe`) untuk sesi debugging visual via tombol **`F5`** di VS Code.

---

### Catatan Diagnostik & Pemecahan Masalah Teknis

1. **Masalah SeaBIOS Hang & Interrupt IRQ0 Tak Terdefinisi**:
   - *Penyebab*: Sesuai Multiboot 1 Spec, status *Interrupt Flag* (IF) di EFLAGS tidak dijamin mati saat bootloader masuk ke `_start`. Jika IF aktif dan IRQ0 (Timer Interrupt) masuk sebelum IDT dipasang via `lidt`, CPU membaca IVT Real-Mode `0x0000` sebagai IDT Protected-Mode 8-byte, merusak register `EAX`, dan menjebak loop `vga_clear`.
   - *Solusi*: Menambahkan instruksi `cli` di baris paling awal `_start` di [boot.asm](file:///c:/project/Nebula/boot/x86_64/boot.asm).

2. **Masalah Crash SSE Vectorization & Alignment Stack**:
   - *Penyebab*: Kompilator `g++ -O2` menghasilkan instruksi vektor SSE (`movdqa`) pada loop `vga_clear`. Karena register CR4.OSFXSR belum diaktifkan dan stack belum 16-byte aligned, CPU memicu exception **General Protection Fault (#GP)**.
   - *Solusi*: Mengaktifkan flag `-mno-sse -mno-sse2 -mno-mmx -mno-80387` pada `CFLAGS` dan mengatur penyeimbangan alignment stack 16-byte di `_start`.

3. **Masalah Relokasi Tambahan Symbol Offset PE-to-ELF (`+4` Bytes Shift)**:
   - *Penyebab*: Pada lingkungan Windows MinGW, kompilasi file C++ terpisah menghasilkan file objek PE/COFF. Konversi ke ELF via `objcopy -I pe-i386 -O elf32-i386` menambahkan addend `+4` byte pada panggilan fungsi `extern "C"`, menyebabkan `call vga_write_at + 4` melompati instruksi pembukaan fungsi.
   - *Solusi*: Menyatukan kompilasi driver C++ secara modular dan menyelaraskan penanganan relokasi simbol di [Makefile](file:///c:/project/Nebula/Makefile).

---

### Hasil Akhir Eksekusi (Verification Status)

- **Mode GUI (`mingw32-make run`)**: Jendela QEMU menampilkan layar biru bersih dengan spanduk spanduk konsol `Nebula OS Kernel - Phase 1 Complete!`.
- **Mode Serial Terminal**: Port COM1 memancarkan log real-time:
  ```text
  [KERNEL] Entered kernel_main()
  [KERNEL] Clearing VGA screen with blue background...
  [KERNEL] Screen cleared successfully!
  [KERNEL] Writing VGA banner...
  [KERNEL] Phase 1 initialization complete! Entering infinite loop...
  ```
- **Mode Visual Debugger (`F5` di VS Code)**: GDB terhubung langsung ke QEMU port 1234, breakpoint `_start` dan `kernel_main` berfungsi 100% lancar.

---

### Status Pengembangan Saat Ini:
- **Phase 1 (Multiboot 1 Kernel & VGA Driver)**: SELESAI (100%)
- **Phase 2 (GDT - Global Descriptor Table)**: Siap Dilanjutkan
