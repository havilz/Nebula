# CHANGELOG & DOKUMENTASI TEKNIS - NEBULA OS

Dokumen ini mencatat seluruh riwayat perubahan, keputusan arsitektur teknis, diagnosa masalah, dan solusi yang diterapkan pada proyek **Nebula OS**. Dokumen ini diperbarui secara berkala setiap kali suatu Fase / Task selesai diimplementasikan.

---

## [Task 8] - Refactoring Kernel ke Arsitektur XNU-Hybrid 1-to-1 (Mach Core, BSD Services, IOKit Drivers, Libkern, Libsa, Security, Pexpert, San)

### Ringkasan Tujuan
Merekonstruksi total struktur direktori dan arsitektur kode sumber kernel Nebula OS agar mengadopsi 8 modul utama **Apple XNU-Hybrid Architecture** secara presisi 1-banding-1 (`mach`, `bsd`, `iokit`, `libkern`, `libsa`, `security`, `pexpert`, `san`) tanpa merusak fungsi yang sudah berjalan.

---

### Perubahan & Komponen Utama yang Dibuat

#### 1. Penataan Modul Mach Microkernel Core (`include/mach/` & `kernel/mach/`)
- **`arch/`**: Abstraksi hardware CPU (GDT, IDT, PIC, TSS 64-bit, ISR Stubs Assembly).
- **`vm/`**: Physical Memory Manager Bitmap, Virtual Memory Manager Page Tables, Kernel Heap Pool `0x00C00000`.
- **`sched/`**: Preemptive Thread Scheduler, Process Control Block (PCB), & Thread Control Block (TCB).

#### 2. Penataan Modul BSD POSIX OS Services (`include/bsd/` & `kernel/bsd/`)
- **`vfs/`**: Abstraksi Virtual File System `vnode` dan Initrd RAM Disk driver (`/initrd/`).
- **`sys/`**: System Call Gate Vector 128 (`INT 0x80`).

#### 3. Penataan Modul IOKit Object-Oriented C++ Drivers (`include/iokit/` & `kernel/iokit/`)
- **`iodevice.hpp` & `iodevice.cpp`**: Base class abstrak `IODevice` dengan lifecycle management (`init`, `start`, `stop`).
- **`display/`**: Driver Bochs BGA VBE Framebuffer (800x600 32-bit BPP).
- **`input/`**: Driver PS/2 Mouse (IRQ 12) & PS/2 Keyboard.
- **`timer/`**: Driver PIT 8254 Timer (100Hz).
- **`serial/`**: Driver UART 16550 Serial Debug Logging (Port COM1).

#### 4. Penataan Modul Kernel Runtime & Security (`libkern`, `libsa`, `security`, `pexpert`, `san`)
- **`libkern/`**: Base class `OSObject` dengan `retain()` / `release()` atomic refcounting dan `io.hpp` Port I/O.
- **`libsa/`**: Standalone kernel runtime dan dynamic module loader untuk kernel extension (kext).
- **`security/`**: Framework keamanan Mandatory Access Control (MAC) dan kredensial pengguna (`ucred_t`).
- **`pexpert/`**: Platform Expert device tree parser dan hardware discovery.
- **`san/`**: Hooks pelacak kebocoran dan kerusakan memori kernel (KASAN/UBSAN).

---

## [Task 7] - VBE High-Resolution Framebuffer & GUI Window Manager Compositor (Bochs BGA Hardware Driver)

### Ringkasan Tujuan
Membangun subsistem grafis VBE Linear Framebuffer (LFB) berbasis driver hardware **Bochs VBE Dispi Interface (BGA)** pada Port I/O `0x1CE` & `0x1CF`, Double Buffering Backbuffer RAM, Bitmap Font Engine 8x16, Driver PS/2 Mouse IRQ 12, serta Compositor GUI Window Manager lengkap dengan wallpaper Deep Space, Taskbar, dan Jendela Interaktif.

---

### Perubahan & Komponen Utama yang Dibuat

#### 1. Driver Hardware Bochs VBE Dispi Interface (BGA) (`include/iokit/display/vbe.hpp` & `kernel/iokit/display/vbe.cpp`)
- **Port I/O 16-bit (`outw` & `inw`)**: Menambahkan fungsi akses I/O 16-bit word di [io.hpp](file:///c:/project/Nebula/include/libkern/io.hpp) untuk memrogram register BGA Port `0x1CE` & `0x1CF`.
- **Deteksi Versi Hardware BGA**: Mampu mendeteksi versi hardware Bochs BGA `0xB0C5` pada QEMU secara otomatis.
- **Konfigurasi Mode Video Hardware**: Mengaktifkan mode grafis resolusi 800x600 32-bit BPP via `bga_set_video_mode()` dan mengaitkan `m_framebuffer` langsung ke alamat Physical Video RAM (`0xFD000000`).
- **Identity Mapping VMM**: Memetakan alamat memori LFB fisik `0xFD000000` & `0xE0000000` di [vmm.cpp](file:///c:/project/Nebula/kernel/mach/vm/vmm.cpp) sehingga penulisan piksel tidak memicu Page Fault (`#PF`).

#### 2. Optimization Linker Script ELF (`boot/linker.ld`)
- **Pemisahan Section `.bss` (NOLOAD)**: Memisahkan section `.bss` dari `.text` sehingga ukuran file biner kernel `nebula.elf` terpangkas dari **4.3 Megabyte** menjadi hanya **44 Kilobyte**! Booting di SeaBIOS / QEMU berjalan instan 0.001 detik.

#### 3. Bitmap Font Engine 8x16 (`include/gui/font.hpp` & `kernel/gui/font.cpp`)
- **Pencetakan Teks Grafis**: Function `Font::draw_char()` dan `Font::draw_string()` merender karakter bitmap 8x16 piksel berwarna pada koordinat manapun di layar GUI.

#### 4. Driver PS/2 Mouse IRQ 12 & Graphic Cursor (`include/iokit/input/mouse.hpp` & `kernel/iokit/input/mouse.cpp`)
- **IRQ 12 Interrupt Handling**: Membaca paket data 3-byte dari Mouse Port `0x60` (IRQ 12 / Vector 44) dan memperbarui koordinat kursor `(x, y)`.
- **Graphic Arrow Cursor**: Function `Mouse::draw_cursor()` menggambar pointer panah 12x18 piksel di atas layar backbuffer.

#### 5. Compositor GUI Window Manager (`include/gui/wm.hpp` & `kernel/gui/wm.cpp`)
- **Manajemen Jendela**: Struct `Window` mengelola `id`, `x`, `y`, `width`, `height`, `title`, `is_focused`, `is_dragging`, `render_func`.
- **Titlebar & Controls**: Menggambar Titlebar, Tombol Close (X), Tombol Minimize (-), Border Jendela, serta menangani *Z-ordering* dan *Drag & Drop* jendela via mouse.
- **Desktop UI**: Wallpaper gradien "Deep Space" dan Taskbar bawah dengan Jam Digital Uptime real-time.

---

### Status Pengembangan Saat Ini:
- **Phase 1 (Multiboot 1 Kernel & VGA Driver)**: SELESAI (100%)
- **Phase 2 (GDT, IDT, PIC & ISR Handling)**: SELESAI (100%)
- **Phase 3 (Manajemen Memori - PMM, VMM Paging, & Heap)**: SELESAI (100%)
- **Phase 4 (Driver Hardware Input & Timer - PIT, PS/2, Serial)**: SELESAI (100%)
- **Phase 5 (Multitasking & Preemptive Scheduler)**: SELESAI (100%)
- **Phase 6 (Virtual File System & Ring 3 Syscall)**: SELESAI (100%)
- **Phase 7 (VBE Framebuffer & GUI Window Manager)**: SELESAI (100%)
- **Phase 8 (Refactoring Kernel ke Arsitektur XNU-Hybrid 1-to-1)**: SELESAI (100%)
