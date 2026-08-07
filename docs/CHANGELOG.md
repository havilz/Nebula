# CHANGELOG & DOKUMENTASI TEKNIS - NEBULA OS

Dokumen ini mencatat seluruh riwayat perubahan, keputusan arsitektur teknis, diagnosa masalah, dan solusi yang diterapkan pada proyek **Nebula OS**. Dokumen ini diperbarui secara berkala setiap kali suatu Fase / Task selesai diimplementasikan.

---

## [Task 7] - VBE High-Resolution Framebuffer & GUI Window Manager Compositor

### Ringkasan Tujuan
Membangun subsistem grafis VBE Linear Framebuffer (LFB), Double Buffering Backbuffer, Bitmap Font Engine 8x16, Driver PS/2 Mouse IRQ 12, serta Compositor GUI Window Manager lengkap dengan wallpaper Deep Space, Taskbar, dan Jendela Interaktif.

---

### Perubahan & Komponen Utama yang Dibuat

#### 1. VBE Linear Framebuffer & Double Buffering (`include/kernel/drivers/vbe.hpp` & `kernel/drivers/gui/vbe.cpp`)
- **Linear Framebuffer (LFB)**: Pengaksesan memori VESA VBE 32-bit True Color (`0xA0000` / LFB address).
- **Double Buffering Engine**: Mengalokasikan backbuffer di Kernel Heap untuk pengoperasian `put_pixel()`, `fill_rect()`, `draw_rect()`, dan `swap_buffers()` tanpa flickering.

#### 2. Bitmap Font Engine 8x16 (`include/kernel/gui/font.hpp` & `kernel/gui/font.cpp`)
- **Pencetakan Teks Grafis**: Function `Font::draw_char()` dan `Font::draw_string()` merender karakter bitmap 8x16 piksel berwarna pada koordinat manapun di layar GUI.

#### 3. Driver PS/2 Mouse IRQ 12 & Graphic Cursor (`include/kernel/drivers/mouse.hpp` & `kernel/drivers/input/mouse.cpp`)
- **IRQ 12 Interrupt Handling**: Membaca paket data 3-byte dari Mouse Port `0x60` (IRQ 12 / Vector 44) dan memperbarui koordinat kursor `(x, y)`.
- **Graphic Arrow Cursor**: Function `Mouse::draw_cursor()` menggambar pointer panah 12x18 piksel di atas layar backbuffer.

#### 4. Compositor GUI Window Manager (`include/kernel/gui/wm.hpp` & `kernel/gui/wm.cpp`)
- **Manajemen Jendela**: Struct `Window` mengelola `id`, `x`, `y`, `width`, `height`, `title`, `is_focused`, `is_dragging`, `render_func`.
- **Titlebar & Controls**: Menggambar Titlebar, Tombol Close (X), Tombol Minimize (-), Border Jendela, serta menangani *Z-ordering* dan *Drag & Drop* jendela via mouse.
- **Desktop UI**: Wallpaper gradien "Deep Space" dan Taskbar bawah dengan Jam Digital Uptime real-time.

#### 5. Pengujian Desktop GUI di Kernel Main (`kernel/core/kernel.cpp`)
- **System Information Window**: Menampilkan arsitektur CPU, alokasi heap, status VFS, dan preemptive scheduler.
- **Terminal Emulator GUI Window**: Menampilkan masukan ketikan keyboard secara real-time pada jendela terminal GUI.

---

### Hasil Akhir Eksekusi (Verification Status)

- **Mode GUI (`mingw32-make run`)**: Jendela QEMU beralih ke Mode Grafis Resolusi Tinggi:
  - Wallpaper Desktop bergradien "Deep Space" dengan spanduk *"Nebula OS - Desktop GUI v1.0"*.
  - Taskbar bawah hitam dengan Tombol *"NEBULA"* dan Jam Digital Uptime real-time.
  - Two Interactive Windows: *System Information* dan *Terminal Console GUI*.
  - Graphic Arrow Mouse Cursor yang meluncur secara *smooth*.
- **Mode Serial Terminal**: Port COM1 memancarkan log inisialisasi subsistem GUI:
  ```text
  [VBE] Framebuffer Driver Initialized (800x600 32-bit BPP)
  [WM] Desktop Compositor Initialized
  [MOUSE] PS/2 Mouse Driver Initialized (IRQ 12)
  [WM] Created GUI Window: System Information
  [WM] Created GUI Window: Terminal Console
  [KERNEL] Phase 7 Desktop GUI Compositor Active!
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

---

## [Task 6] - Virtual File System (VFS), Initrd RAM Disk, & System Call Infrastructure (INT 0x80)

### Ringkasan Tujuan
Membangun abstraksi Virtual File System (VFS `vnode_t`), driver Initrd RAM Disk dalam memori (`/initrd/hello.txt`), serta infrastruktur System Call DPL 3 via IDT Vector 128 (`INT 0x80`).
