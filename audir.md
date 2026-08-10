# Nebula OS — Code Audit Report

**Repository:** havilz/Nebula
**Branch:** `dev`
**Commit reviewed:** `4847d5d` — "feat(net): implement Intel E1000 driver and BSD TCP/IP protocol stack"
**Tanggal audit:** 10 Agustus 2026
**Metode:** Clone langsung dari repo, inspeksi source code per-file, build verification, tanpa asumsi dari commit message.

---

## Ringkasan Eksekutif

Project mengalami lompatan scope besar dalam waktu singkat: dari Phase 7 (GUI/VBE) menuju restrukturisasi penuh ke arsitektur bergaya **XNU** (kernel macOS/Darwin) — `mach/`, `bsd/`, `iokit/`, `libkern/`, `libsa/`, `pexpert/`, `san/`, `security/` — ditambah klaim ELF loader, POSIX syscall layer, driver ATA/FAT32, driver Intel E1000, dan BSD TCP/IP stack.

Audit ini memverifikasi setiap klaim langsung dari source code (bukan dari nama commit atau nama folder), dan menemukan **campuran nyata antara implementasi fungsional dan stub/placeholder yang dilabeli seolah lengkap**. Selain itu, **build saat ini gagal link** (`undefined symbol: __cxa_atexit`) — hal ini terpisah dari isu "stuck di SeaBIOS" yang sebelumnya dibahas.

---

## 1. Status Build

| Item | Hasil |
|---|---|
| `make clean && make all` | ❌ **Gagal** — `ld.lld: error: undefined symbol: __cxa_atexit` |
| Sumber masalah | Dua object `static` lokal di dalam `kernel_main()`: `static nebula::drivers::ATADriver ata_drive;` dan `static nebula::drivers::E1000Driver e1000_nic;`. Object `static` lokal dengan destructor non-trivial di C++ memerlukan `__cxa_atexit` untuk registrasi destructor — fungsi ini tidak tersedia di runtime freestanding. |
| Fix yang diverifikasi | Menambahkan stub kosong `__cxa_atexit`/`__dso_handle` (aman karena `kernel_main()` tidak pernah `return`, destructor tidak akan pernah butuh dipanggil). Setelah stub ditambahkan di clone lokal audit, build sukses. **Perubahan ini tidak di-commit ke repo — murni untuk verifikasi.** |

---

## 2. Temuan per Komponen

### ✅ Genuinely Implemented (logic nyata, terverifikasi dari source)

| Komponen | File | Catatan |
|---|---|---|
| ELF Loader | `kernel/bsd/sys/elf.cpp` | Validasi magic & machine type, baca program header table via VFS, iterasi segment `PT_LOAD`, alokasi memori, baca data segment dari file, zero-fill BSS. **Catatan risiko:** segment di-load ke heap kernel biasa (`kmalloc`), bukan di-*map* ke `p_vaddr` asli lewat page table — berisiko salah alamat kecuali code position-independent. |
| Syscall Dispatcher | `kernel/bsd/sys/syscall.cpp` | `SYS_WRITE`, `SYS_READ`, `SYS_YIELD`, `SYS_EXIT` benar-benar terhubung ke subsistem nyata (`Serial::write_string`, `VFS::read`, `Scheduler::yield`). |
| Ethernet byte-order | `kernel/bsd/net/ethernet.cpp` | `htons`/`ntohs`/`htonl`/`ntohl` — implementasi benar secara teknis. |
| IPv4 Checksum | `kernel/bsd/net/ipv4.cpp` | Algoritma checksum ones'-complement — matematis valid. |
| FAT32 Mount | `kernel/bsd/vfs/fat32.cpp` | Parsing BIOS Parameter Block (BPB) dan perhitungan lokasi FAT/cluster start — benar. |

### ⚠️ Partially Real (pola/struktur ada, penerapan tidak konsisten/tidak lengkap)

| Komponen | File | Catatan |
|---|---|---|
| IOKit Base Class | `kernel/iokit/iodevice.cpp`, `include/iokit/iodevice.hpp` | Class `IODevice` (virtual `init`/`start`/`stop`) memang ada — bukan sekadar nama. Namun **hanya dipakai 2 dari 8 driver** (`ata.cpp`, `e1000.cpp`). Driver lama (`vga`, `vbe`, `keyboard`, `mouse`, `serial`, `pit`) hanya dipindah folder ke `iokit/` tanpa refactor mengikuti pattern baru. |
| Driver Matching/Registry | — | Tidak ditemukan sistem device-tree/registry/`probe()`-based matching — fitur inti arsitektur IOKit asli. |

### ❌ Stub / Overclaimed (nama/commit message menyatakan lebih dari yang diimplementasikan)

| Komponen | File | Catatan |
|---|---|---|
| Mach Architecture | `kernel/mach/` | Seluruh isi folder adalah file lama (`gdt.cpp`, `idt.cpp`, `interrupts.cpp`, `pic.cpp`, `tss.cpp`, `scheduler.cpp`, `heap.cpp`, `pmm.cpp`, `vmm.cpp`) yang **dipindah folder tanpa perubahan fungsional**. Tidak ada Mach IPC, ports, message queue, atau task/thread separation — ciri khas arsitektur Mach yang sebenarnya. |
| BSD Socket Layer | `kernel/bsd/net/socket.cpp` | `sys_socket()` → `return 3;` dengan komentar eksplisit `// Simulated socket descriptor`. `sys_bind()` → `return 0;` tanpa logic. Tidak ada state machine koneksi, tidak ada transfer data. |
| TCP/IP Stack | `kernel/bsd/net/` | **Tidak ada file `tcp.cpp` atau `udp.cpp` sama sekali**, meski commit message menyatakan "BSD TCP/IP protocol stack". |
| IPv4 Packet Dispatch | `kernel/bsd/net/ipv4.cpp` — `handle_ip()` | Menerima paket, cek panjang, lalu **hanya mencetak log** `"Incoming IPv4 Packet Received"`. Tidak membaca field protocol, tidak dispatch ke ICMP/TCP, tidak validasi checksum, tidak memproses payload. |
| IPv4 Address Assignment | `kernel/bsd/net/ipv4.cpp` — `init()` | Mencetak string IP hardcoded (`"10.0.2.15"` — kebetulan cocok default QEMU user-mode networking), bukan hasil DHCP/negosiasi protokol nyata. |
| FAT32 Directory Lookup | `kernel/bsd/vfs/fat32.cpp` — `finddir()` | **`return nullptr;` tanpa logic apapun.** Konsekuensi: file tidak bisa dibuka berdasarkan nama/path — fungsi paling dasar dari sebuah filesystem belum berfungsi. |
| FAT32 Cluster Chain | `kernel/bsd/vfs/fat32.cpp` — `read()`/`write()` | Hanya menangani **satu cluster**. Tidak ada FAT table walk/traversal — file yang lebih besar dari satu cluster akan terpotong/corrupt. |

---

## 3. Rekomendasi Prioritas

1. **`FAT32::finddir()`** — prioritas tinggi kalau ingin filesystem benar-benar dipakai; tanpa ini FAT32 secara praktis tidak fungsional meski `mount()` valid.
2. **Selaraskan commit message dengan isi implementasi** — khususnya untuk `mach/` (relabel folder, bukan arsitektur Mach) dan `net/socket.cpp` (simulasi, bukan TCP/IP stack). Ini bukan soal "salah", tapi soal akurasi dokumentasi/histori project — penting untuk LPDP/portofolio dan untuk AI agentic berikutnya agar tidak membangun di atas asumsi yang keliru soal apa yang sudah "selesai".
3. **Refactor driver lama ke `IODevice` base class** secara konsisten, atau dokumentasikan eksplisit bahwa migrasi baru sebagian.
4. **IPv4 dispatch logic** — sambungkan `handle_ip()` ke handler protocol yang sesuai (ICMP sudah ada file-nya, tinggal disambungkan).

---

## 4. Catatan Metodologi

Semua temuan di atas diverifikasi langsung dari isi source code (bukan dari nama file, nama folder, atau commit message), termasuk dengan build aktual di sandbox terpisah. Tidak ada perubahan yang di-push ke repository — audit ini bersifat read-only terhadap repo GitHub kamu; satu-satunya modifikasi (stub `__cxa_atexit`) dilakukan di clone lokal semata untuk memverifikasi bahwa fix tersebut benar-benar menyelesaikan error link, dan tidak disimpan/dikirim kembali ke repo.