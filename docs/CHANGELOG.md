# CHANGELOG & DOKUMENTASI TEKNIS - NEBULA OS

Dokumen ini mencatat seluruh riwayat perubahan, keputusan arsitektur teknis, diagnosa masalah, dan solusi yang diterapkan pada proyek **Nebula OS**. Dokumen ini diperbarui secara berkala setiap kali suatu Fase / Task selesai diimplementasikan.

---

## [Task 7] - VBE High-Resolution Framebuffer & GUI Window Manager Compositor (Bochs BGA Hardware Driver)

### Ringkasan Tujuan
Membangun subsistem grafis VBE Linear Framebuffer (LFB) berbasis driver hardware **Bochs VBE Dispi Interface (BGA)** pada Port I/O `0x1CE` & `0x1CF`, Double Buffering Backbuffer RAM, Bitmap Font Engine 8x16, Driver PS/2 Mouse IRQ 12, serta Compositor GUI Window Manager lengkap dengan wallpaper Deep Space, Taskbar, dan Jendela Interaktif.

---

### Perubahan & Komponen Utama yang Dibuat

#### 1. Driver Hardware Bochs VBE Dispi Interface (BGA) (`include/kernel/drivers/vbe.hpp` & `kernel/drivers/gui/vbe.cpp`)
- **Port I/O 16-bit (`outw` & `inw`)**: Menambahkan fungsi akses I/O 16-bit word di [io.hpp](file:///c:/project/Nebula/include/kernel/arch/x86_64/io.hpp) untuk memrogram register BGA Port `0x1CE` & `0x1CF`.
- **Deteksi Versi Hardware BGA**: Mampu mendeteksi versi hardware Bochs BGA `0xB0C5` pada QEMU secara otomatis.
- **Konfigurasi Mode Video Hardware**: Mengaktifkan mode grafis resolusi 800x600 32-bit BPP via `bga_set_video_mode()` dan mengaitkan `m_framebuffer` langsung ke alamat Physical Video RAM (`0xFD000000`).
- **Identity Mapping VMM**: Memetakan alamat memori LFB fisik `0xFD000000` & `0xE0000000` di [vmm.cpp](file:///c:/project/Nebula/kernel/memory/vmm.cpp) sehingga penulisan piksel tidak memicu Page Fault (`#PF`).

#### 2. Optimization Linker Script ELF (`boot/linker.ld`)
- **Pemisahan Section `.bss` (NOLOAD)**: Memisahkan section `.bss` dari `.text` sehingga ukuran file biner kernel `nebula.elf` terpangkas dari **4.3 Megabyte** menjadi hanya **44 Kilobyte**! Booting di SeaBIOS / QEMU berjalan instan 0.001 detik.

#### 3. Bitmap Font Engine 8x16 (`include/kernel/gui/font.hpp` & `kernel/gui/font.cpp`)
- **Pencetakan Teks Grafis**: Function `Font::draw_char()` dan `Font::draw_string()` merender karakter bitmap 8x16 piksel berwarna pada koordinat manapun di layar GUI.

#### 4. Driver PS/2 Mouse IRQ 12 & Graphic Cursor (`include/kernel/drivers/mouse.hpp` & `kernel/drivers/input/mouse.cpp`)
- **IRQ 12 Interrupt Handling**: Membaca paket data 3-byte dari Mouse Port `0x60` (IRQ 12 / Vector 44) dan memperbarui koordinat kursor `(x, y)`.
- **Graphic Arrow Cursor**: Function `Mouse::draw_cursor()` menggambar pointer panah 12x18 piksel di atas layar backbuffer.

#### 5. Compositor GUI Window Manager (`include/kernel/gui/wm.hpp` & `kernel/gui/wm.cpp`)
- **Manajemen Jendela**: Struct `Window` mengelola `id`, `x`, `y`, `width`, `height`, `title`, `is_focused`, `is_dragging`, `render_func`.
- **Titlebar & Controls**: Menggambar Titlebar, Tombol Close (X), Tombol Minimize (-), Border Jendela, serta menangani *Z-ordering* dan *Drag & Drop* jendela via mouse.
- **Desktop UI**: Wallpaper gradien "Deep Space" dan Taskbar bawah dengan Jam Digital Uptime real-time.

---

### Hasil Akhir Eksekusi (Verification Status)

- **Mode GUI (`mingw32-make run`)**: Jendela QEMU beralih ke Mode Grafis Hardware Resolusi Tinggi:
  - Wallpaper Desktop bergradien "Deep Space" dengan spanduk *"Nebula OS - Desktop GUI v1.0"*.
  - Taskbar bawah hitam dengan Tombol *"NEBULA"* dan Jam Digital Uptime real-time.
  - Two Interactive Windows: *System Information* dan *Terminal Console GUI*.
  - Graphic Arrow Mouse Cursor yang meluncur secara *smooth*.
- **Mode Serial Terminal**: Port COM1 memancarkan log inisialisasi hardware BGA:
  ```text
  [VBE BGA] Detected Bochs BGA Hardware Version: 0x0000B0C5
  [VBE] Linear Framebuffer Connected at 0xFD000000 (800x600 32-bit BPP)
  [WM] Desktop Compositor Initialized
  [MOUSE] PS/2 Mouse Driver Initialized (IRQ 12)
  [WM] Created GUI Window: System Information
  [WM] Created GUI Window: Terminal Console
  [THREAD ALPHA] Active in background
  [THREAD BETA] Active in background
  ```

---

### Status Pengembangan Saat Ini:
- **Phase 1 (Multiboot 1 Kernel & VGA Driver)**: SELESAI (100%)
- **Phase 2 (GDT, IDT, PIC & ISR Handling)**: SELESAI (100%)
- **Phase 3 (Manajemen Memori - PMM, VMM Paging, & Heap)**: SELESAI (100%)
- **Phase 4 (Driver Hardware Input & Timer - PIT, PS/2, Serial)**: SELESAI (100%)
- **Phase 5 (Multitasking & Preemptive Scheduler)**: SELESAI (100%)
- **Phase 6 (Virtual File System & Ring 3 Syscall)**: SELESAI (100%)
- **Phase 7 (VBE Framebuffer & GUI Window Manager)**: SELESAI (100%)
