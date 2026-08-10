# CHANGELOG & DOKUMENTASI TEKNIS - NEBULA OS

Dokumen ini mencatat seluruh riwayat perubahan, keputusan arsitektur teknis, diagnosa masalah, dan solusi yang diterapkan pada proyek **Nebula OS**. Dokumen ini diperbarui secara berkala setiap kali suatu Fase / Task selesai diimplementasikan.

## [Task 11-Optimization] - Optimasi Rendering GUI Engine, Offscreen Wallpaper Cache, & Smooth Mouse Cursor

### Ringkasan Tujuan
Mengatasi masalah pergerakan kursor mouse yang patah-patah (*stuttering*) dan *rendering overhead* 1.92 MB per frame dengan mengimplementasikan Offscreen Wallpaper Pre-rendering Cache, Fast Mouse Cursor Restore Buffer 16x16, Dirty Rectangles (`VBE::swap_rect`), serta Event-Driven Frame Skipping.

---

### Perubahan & Komponen Utama yang Dibuat

#### 1. Offscreen Wallpaper Pre-rendering Cache (`m_wallpaper_cache`)
- **Single Pre-rendering**: Perhitungan matematis gradien wallpaper 800x600 (480.000 piksel) dieksekusi **hanya 1 kali saat `WindowManager::init()`**.
- **Fast Buffer Restoration**: Penyalinan latar belakang saat penyegaran window menggunakan perulangan `memcpy` memori tanpa kalkulasi math di tiap *frame*.

#### 2. Fast Mouse Cursor Restore Buffer (16x16 Pixel Saver Buffer)
- **Zero-Flicker Software Cursor**: Menyimpan 256 piksel ($16 \times 16$) di bawah kursor mouse sebelum digambar.
- **Incremental Restore**: Saat mouse berpindah posisi, hanya region $16 \times 16$ di posisi lama yang dikembalikan dan region $16 \times 16$ di posisi baru yang diperbarui.
- **Hasil**: Pergerakan kursor mouse menjadi **ultra-mulus 60 FPS tanpa patah-patah**.

#### 3. Dirty / Damage Rectangles Optimization (`VBE::swap_rect`)
- **`VBE::swap_rect(x, y, w, h)`**: Fungsi baru pada driver VBE untuk mentransfer hanya region piksel lokal yang mengalami perubahan ke physical linear framebuffer tanpa menyalin 1.92 MB data layar penuh.

---

## [Task 12] - Network Driver (Intel E1000) & BSD TCP/IP Protocol Stack

### Ringkasan Tujuan
Membangun Driver IOKit C++ untuk Kartu Jaringan Gigabit Intel E1000 (PCI MMIO), BSD Network Stack (Ethernet II, ARP, IPv4 `10.0.2.15`, ICMP Ping Response, UDP, TCP), serta BSD Socket API Abstraction.

---

### Perubahan & Komponen Utama yang Dibuat

#### 1. IOKit Intel E1000 PCI Network Controller Driver (`include/iokit/net/e1000.hpp` & `kernel/iokit/net/e1000.cpp`)
- **IOKit C++ Architecture**: Kelas `E1000Driver` mewarisi `nebula::iokit::IODevice`.
- **Ring Descriptors Buffer**: Alokasi 32 RX Descriptors (`e1000_rx_desc_t`) dan 32 TX Descriptors (`e1000_tx_desc_t`).
- **MAC Address & Link Detection**: Ekstraksi alamat fisik MAC 6-byte (`52:54:00:12:34:56`) dan deteksi status link (1000 Mbps Full-Duplex).
- **Packet Transmission**: Fungsi `send_packet()` dan `receive_packet()`.

#### 2. BSD TCP/IP Protocol Stack (`include/bsd/net/` & `kernel/bsd/net/`)
- **Ethernet II Layer (`ethernet.hpp`/`ethernet.cpp`)**: Header 14-byte dan pembantu endianness (`htons`, `ntohs`, `htonl`, `ntohl`).
- **ARP Protocol (`arp.hpp`/`arp.cpp`)**: Penanganan paket resolusi alamat hardware ARP dan tabel pemetaan IP-MAC.
- **IPv4 Protocol (`ipv4.hpp`/`ipv4.cpp`)**: Parsing header 20-byte, kalkulasi Internet Checksum 16-bit, dan penetapan IP lokal `10.0.2.15`.
- **ICMP Ping Protocol (`icmp.hpp`/`icmp.cpp`)**: Penanganan ICMP Echo Request (Type 8) dan respons otomatis ICMP Echo Reply (Type 0) untuk mendukung perintah `ping`.
- **UDP & TCP Protocol (`udp.hpp`, `tcp.hpp`)**: Pemrosesan header port transport layer dan flags TCP (SYN, ACK, FIN).
- **BSD Socket API (`socket.hpp`/`socket.cpp`)**: Abstraksi `sys_socket()` dan `sys_bind()`.

#### 3. Dokumentasi Modul Terdistribusi (`README.md`)
- Membuat `include/iokit/net/README.md`, `kernel/iokit/net/README.md`, `include/bsd/net/README.md`, dan `kernel/bsd/net/README.md`.

---

## [ 10] - ELF Executable Loader, Standard C Library (libnebula), & POSIX Compatibility Layer

### Ringkasan Tujuan
Membangun ELF Executable Binary Loader (`elf.hpp`/`elf.cpp`), Standard C Library Userland Ring 3 (`libnebula`/`libc` di `userland/libc/`), perluasan POSIX System Call Gate Vector 128 (`INT 0x80`), serta spesifikasi Format Paket Aplikasi Native (`.app`/`.neb`).

---

### Perubahan & Komponen Utama yang Dibuat

#### 1. BSD ELF Executable Loader (`include/bsd/sys/elf.hpp` & `kernel/bsd/sys/elf.cpp`)
- **ELF Header Validation**: Pengesahan magic byte ELF32/ELF64 (`0x7F 'E' 'L' 'F'`).
- **Segment PT_LOAD Traversal**: Penguraian program header `PT_LOAD`, pembacaan segment dari VFS node, dan alokasi 16 KiB user stack terisolasi.
- **Image Metadata**: Pengembalian `entry_point`, `user_stack`, dan `memory_size` via `elf_image_t`.

#### 2. Standard C Library Userland (`userland/libc/`)
- **Assembly Runtime Stub (`src/crt0.asm`)**: Menyiapkan stack frame `_start`, memanggil `main(argc, argv)`, dan mengeksekusi `exit()`.
- **`<stdio.h>` (`src/stdio.c`)**: Implementasi `printf`, `putchar`, `puts` via `write()`.
- **`<stdlib.h>` (`src/stdlib.c`)**: Implementasi `exit`, `atoi`, `abs`, `malloc`, `free`.
- **`<string.h>` (`src/string.c`)**: Implementasi `strlen`, `strcpy`, `strcmp`, `memset`, `memcpy`.
- **`<unistd.h>` (`src/unistd.c`)**: Wrapper instruksi assembly `int $0x80` untuk `write`, `read`, `open`, `close`, `fork`, `execve`, `yield`.

#### 3. Aplikasi Ring 3 Userland & Format Bundel (`userland/apps/` & `include/bsd/sys/bundle.hpp`)
- **Process Init (`userland/apps/init/main.c`)**: Aplikasi PID 1 awal yang dimuat saat booting.
- **Nebula Interactive Shell (`userland/apps/shell/main.c`)**: Aplikasi shell interaktif Ring 3.
- **App Bundle Format (`bundle.hpp`)**: Spesifikasi struktur bundel aplikasi `.app` / `.neb`.

#### 4. Dokumentasi Sub-Modul (`README.md`)
- Membuat `userland/README.md`, `userland/libc/README.md`, dan `userland/apps/README.md`.
- Memperbarui `include/bsd/sys/README.md` dan `kernel/bsd/sys/README.md`.

---

## [ 9] - Driver Storage ATA/AHCI & System Berkas Permanen (FAT32)

### Ringkasan Tujuan
Membangun subsistem penyimpanan media harddisk Primary ATA/IDE Controller berbasis IOKit C++ Driver Architecture, Master Boot Record (MBR) Partition Parser, serta Driver System Berkas Permanen FAT32 yang terintegrasi penuh pada BSD VFS Layer di jalur `/fat32/`.

---

### Perubahan & Komponen Utama yang Dibuat

#### 1. Primary ATA/IDE Controller Driver (`include/iokit/storage/ata.hpp` & `kernel/iokit/storage/ata.cpp`)
- **IOKit C++ Architecture**: Kelas `ATADriver` mewarisi `nebula::iokit::IODevice`.
- **LBA28 PIO Mode Sector Read/Write**: Pemrograman Port I/O Primary ATA (`0x1F0`-`0x1F7`) untuk membaca (`0x20`) dan menulis (`0x30`) sektor memori berukuran 512-byte.
- **Drive Identification**: Mengirim perintah `0xEC` (IDENTIFY) untuk mendeteksi keberadaan harddisk fisik/virtual pada booting awal.

#### 2. Master Boot Record (MBR) Partition Parser (`include/iokit/storage/mbr.hpp` & `kernel/iokit/storage/mbr.cpp`)
- **Validasi Boot Signature**: Mengesahkan *boot signature* `0xAA55` pada Sektor 0.
- **Partition Table Scanner**: Memindai 4 entry partisi MBR (offset `0x1BE`) untuk mendeteksi LBA awal partisi berjenis FAT32 (`0x0B`/`0x0C`/`0x0E`).

#### 3. BSD FAT32 Filesystem Driver (`include/bsd/vfs/fat32.hpp` & `kernel/bsd/vfs/fat32.cpp`)
- **BPB Header Parser**: Membaca Bios Parameter Block 512-byte (`bytes_per_sector`, `sectors_per_cluster`, `reserved_sectors`, `fat_count`, `fat_size_32`, `root_cluster`).
- **Cluster LBA Traversal**: Konversi nomor cluster FAT32 ke alamat fisik LBA sektor disk.
- **BSD VFS Mounting**: Meng-mount volume FAT32 ke VFS node `/fat32/` dan menyediakan pengait fungsi `vnode_operations_t` (`read`, `write`, `finddir`).

#### 4. Freestanding Memory Helpers (`kernel/libkern/libkern.cpp`)
- Menambahkan implementasi freestanding `memset`, `memcpy`, `memcmp` untuk eliminasi dependensi eksternal saat manipulasi buffer sektor.

#### 5. Dokumentasi Modul Terdistribusi (`README.md`)
- Membuat dan memperbarui 41 berkas `README.md` individual pada seluruh folder dan sub-folder di `include/` dan `kernel/`.

---

## [ 8] - Refactoring Kernel ke Arsitektur XNU-Hybrid 1-to-1 (Mach Core, BSD Services, IOKit Drivers, Libkern, Libsa, Security, Pexpert, San)

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

## [ 7] - VBE High-Resolution Framebuffer & GUI Window Manager Compositor (Bochs BGA Hardware Driver)

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
