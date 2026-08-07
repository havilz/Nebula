# CHANGELOG & DOKUMENTASI TEKNIS - NEBULA OS

Dokumen ini mencatat seluruh riwayat perubahan, keputusan arsitektur teknis, diagnosa masalah, dan solusi yang diterapkan pada proyek **Nebula OS**. Dokumen ini diperbarui secara berkala setiap kali suatu Fase / Task selesai diimplementasikan.

---

## [Task 3] - Physical Memory Manager (PMM), Virtual Memory (VMM Paging), & Kernel Heap Allocator

### Ringkasan Tujuan
Membangun subsistem manajemen memori lengkap: Physical Memory Manager (PMM Bitmap 4 KiB frame), Virtual Memory Manager (VMM Identity Paging 16 MB), Kernel Heap Allocator (`kmalloc`/`kfree`), serta overloading operator global C++ `new` dan `delete`.

---

### Perubahan & Komponen Utama yang Dibuat

#### 1. Multiboot Header & Information (`include/kernel/multiboot.h`)
- **Struktur Memori**: Struct `multiboot_info_t` & `multiboot_memory_map_t` untuk membaca informasi RAM fisik dari Multiboot 1 bootloader (`EBX`).

#### 2. Physical Memory Manager (`include/kernel/memory/pmm.hpp` & `kernel/memory/pmm.cpp`)
- **Bitmap Allocator**: Pelacakan status frame fisik 4 KiB per bit (`bitmap_set`, `bitmap_clear`, `bitmap_test`).
- **Parsing Multiboot Mmap**: Membaca region memori fisik yang tersedia (`type == 1`) dan menandai 1 MB dasar kernel sebagai *reserved*.
- **API Utama**: `PMM::init()`, `PMM::allocate_frame()`, `PMM::free_frame()`, `PMM::get_free_memory_kb()`.

#### 3. Virtual Memory Manager & Paging (`include/kernel/memory/vmm.hpp` & `kernel/memory/vmm.cpp`)
- **Tabel Paging 32-bit**: Struct `page_directory_t` & `page_table_t` (1024 PDE & 1024 PTE).
- **Identity Mapping 16 MB**: Memetakan alamat virtual `0x00000000` - `0x01000000` ke alamat fisik (termasuk VGA Buffer `0xB8000`).
- **Pengaktifan Paging**: Memuat `CR3` via `switch_page_directory()` dan mengeset Bit 31 (`PG`) pada register `CR0`.
- **Manajemen Halaman**: `VMM::map_page()` & `VMM::unmap_page()` dengan pembersihan TLB via `invlpg`.

#### 4. Kernel Heap Allocator (`include/kernel/memory/heap.hpp` & `kernel/memory/heap.cpp`)
- **Algoritma Heap**: First-Fit memory block allocation dengan *coalescing* (penggabungan blok bebas berdampingan saat `kfree`).
- **Alokasi Dinamis**: `KernelHeap::init(0x00C00000, 1MB)`, `kmalloc(size)`, dan `kfree(ptr)`.

#### 5. C++ Global Operator `new` / `delete` (`kernel/memory/kheap.cpp`)
- Overloading operator global C++: `operator new`, `operator new[]`, `operator delete`, `operator delete[]` memanggil `kmalloc()` dan `kfree()`.

#### 6. Integrasi Kernel Main (`kernel/core/kernel.cpp`) & Boot Assembly (`boot/x86_64/boot.asm`)
- **`boot.asm`**: Mendorong `ebx` (multiboot info pointer) dan `eax` (magic number) sebagai argumen `kernel_main`.
- **`kernel.cpp`**: Mengurutkan panggilan inisialisasi PMM -> VMM Paging -> Kernel Heap -> Pengujian `kmalloc(512)` -> Pengujian C++ `new TestMemoryObject()`.

---

### Catatan Diagnostik & Pemecahan Masalah Teknis

1. **Bug Penempatan Atribut C++11 `alignas` pada Static Variables**:
   - *Penyebab*: `static alignas(4096) page_directory_t initial_page_directory;` memicu syntax error pada GCC.
   - *Solusi*: Mengubah posisi penulisan specifier menjadi `alignas(4096) static page_directory_t initial_page_directory;`.

---

### Hasil Akhir Eksekusi (Verification Status)

- **Mode GUI (`mingw32-make run`)**: Jendela QEMU menampilkan spanduk konsol Phase 3:
  ```text
  [OK] PMM Bitmap Frame Allocator Initialized
  [OK] VMM Identity Paging (16 MB) & CR0.PG Active
  [OK] Kernel Heap Allocator (0x00C00000 / 1 MB)
  [OK] kmalloc(512) Dynamic Allocation Success
  [OK] C++ Global operator new & delete Success
  Phase 3 Initialization Complete!
  ```
- **Mode Serial Terminal**: Port COM1 memancarkan log real-time:
  ```text
  [PMM] Initializing Physical Memory Manager (Bitmap Allocator)...
  [PMM] PMM Initialized Successfully!
  [VMM] Initializing Virtual Memory Manager & Identity Paging (0-16MB)...
  [VMM] CPU Paging Enabled Safely (CR3 & CR0.PG Active)!
  [HEAP] Initializing Kernel Heap Allocator at 0x00C00000 (1 MB pool)...
  [HEAP] Kernel Heap Initialized Successfully!
  [TEST] Testing kmalloc(512)...
  [TEST] kmalloc(512) Allocation Successful!
  [TEST] kfree(ptr1) Freed Successfully!
  [TEST] Testing C++ Global Operator new...
  [TEST] C++ Operator new TestMemoryObject Success!
  [TEST] delete obj Freed Successfully!
  [KERNEL] Phase 3 initialization complete! Entering infinite loop...
  ```

---

### Status Pengembangan Saat Ini:
- **Phase 1 (Multiboot 1 Kernel & VGA Driver)**: SELESAI (100%)
- **Phase 2 (GDT, IDT, PIC & ISR Handling)**: SELESAI (100%)
- **Phase 3 (Manajemen Memori - PMM, VMM Paging, & Heap)**: SELESAI (100%)
- **Phase 4 (Driver Hardware Input & Timer - PIT, PS/2, Serial)**: Siap Dilanjutkan

---

## [Task 2] - GDT, IDT, 8259 PIC Remapping, & ISR Interrupt Handling

### Ringkasan Tujuan
Membangun fondasi manajemen segmen memori (GDT) dan subsistem penanganan interupsi CPU/Hardware (IDT 256 gates, 8259 PIC Remapping, ISR Exception Stubs, dan IRQ Dispatcher C++), serta mengaktifkan interupsi hardware secara aman via `sti`.

---

### Perubahan & Komponen Utama yang Dibuat

#### 1. Subsystem GDT (`include/kernel/arch/x86_64/gdt.hpp` & `kernel/arch/x86_64/gdt/gdt.cpp`)
- **Struktur Descriptor**: Struct `GDTEntry` (8 byte) & `GDTPointer` (6 byte `limit` & `base`).
- **Peta Segmen**:
  - `0x00`: Null Descriptor
  - `0x08`: Kernel Code Segment (Ring 0, Executable/Read, 4GB Limit)
  - `0x10`: Kernel Data Segment (Ring 0, Read/Write, 4GB Limit)
  - `0x18`: User Code Segment (Ring 3, Executable/Read, 4GB Limit)
  - `0x20`: User Data Segment (Ring 3, Read/Write, 4GB Limit)
- **Rutin `gdt_flush` Inline Assembly**: Memuat GDTR (`lgdt`), melakukan *Far Jump* `ljmp $0x08, $1f` untuk reload `CS` selector, dan mengisi `DS, ES, FS, GS, SS` dengan `0x10`.

#### 2. Subsystem IDT (`include/kernel/arch/x86_64/idt.hpp` & `kernel/arch/x86_64/idt/idt.cpp`)
- **Struktur Gate**: Struct `IDTEntry` (8 byte: `base_low`, selector `0x08`, `always0`, flags `0x8E`, `base_high`) & `IDTPointer` (6 byte).
- **Inisialisasi 256 Entry**: Memasang 256 gate handler dan memuat register IDTR CPU via `asm volatile ("lidt %0")`.

#### 3. Subsystem 8259 PIC Remapping (`include/kernel/arch/x86_64/pic.hpp` & `kernel/arch/x86_64/interrupts/pic.cpp`)
- **Remapping Vector**: Menggeser IRQ hardware 0-15 dari offset default konflik 0-15 ke offset **32-47** (Master PIC IRQ 0-7 -> Vektor 32-39, Slave PIC IRQ 8-15 -> Vektor 40-47).
- **Komunikasi Port I/O**: Mengirimkan ICW1-ICW4 ke Master (`0x20`/`0x21`) dan Slave (`0xA0`/`0xA1`) dengan jeda `io_wait()` pada port `0x80`.
- **EOI Signalling**: Implementasi `PIC::send_eoi(irq)` untuk menutup interupsi.

#### 4. ISR Assembly Stubs & C++ Dispatcher (`kernel/arch/x86_64/interrupts/isr.asm`, `include/kernel/arch/x86_64/interrupts.hpp`, `kernel/arch/x86_64/interrupts/interrupts.cpp`)
- **32 Exception Stubs**: Macro `ISR_NOERRCODE` & `ISR_ERRCODE` memicu `isr0`..`isr31`.
- **16 IRQ Stubs**: Macro `IRQ` memicu `irq0`..`irq15` (vektor 32-47).
- **Common Stubs**: `isr_common_stub` & `irq_common_stub` melakukan `pusha`, memuat `DS=0x10`, memanggil C++ `isr_handler()` / `irq_handler()`, `popa`, dan `iret`.
- **Software Interrupt Test**: Memasang callback `breakpoint_handler` pada Vektor 3 (`INT 3`) dan memunculkan notifikasi sukses pada VGA Console & Serial COM1.

#### 5. Integrasi Kernel Main (`kernel/core/kernel.cpp`) & Build System (`Makefile`)
- Merangkai urutan inisialisasi: `Serial::init()` -> `GDT::init()` -> `IDT::init()` -> `PIC::remap(32, 40)` -> `interrupts_init()` -> **`asm volatile ("sti")`** -> Test `asm volatile ("int $3")`.
- Memperbarui `Makefile` untuk merakit `isr.o` dan menghubungkannya ke `nebula.elf`.

---

### Catatan Diagnostik & Pemecahan Masalah Teknis

1. **Bug Bitwise OR Granularity pada `set_gate`**:
   - *Penyebab*: `m_gdt[num].granularity |= gran` menyimpan bit sampah jika array `m_gdt` belum dibersihkan.
   - *Solusi*: Mengatur `granularity` via penetapan langsung (`=`) dan mengosongkan seluruh array GDT di awal `GDT::init()`.

2. **Pergeseran Symbol Offset `gdt_flush` Assembly (+4 Bytes)**:
   - *Penyebab*: Fungsi assembly terpisah `gdt_flush` di `boot.asm` mengalami pergeseran `+4` byte addend saat dipanggil dari file C++ yang dikonversi dari PE ke ELF, melompati instruksi `mov eax, [esp+4]`.
   - *Solusi*: Memindahkan `gdt_flush` menjadi *inline assembly* di dalam `gdt.cpp` sehingga dieksekusi secara langsung tanpa pergeseran relokasi.

---

### Hasil Akhir Eksekusi (Verification Status)

- **Mode GUI (`mingw32-make run`)**: Jendela QEMU menampilkan layar biru dengan spanduk spanduk konsol Phase 2:
  ```text
  [OK] GDT 32-bit Reloaded (Code:0x08, Data:0x10)
  [OK] IDT 256 Gates & LIDT Loaded
  [OK] 8259 PIC Remapped (IRQ 0-15 -> Vectors 32-47)
  [OK] CPU Interrupts Enabled (STI Active)
  [OK] INT 3 Software Interrupt Handler Triggered
  Phase 2 Initialization Complete!
  ```
- **Mode Serial Terminal**: Port COM1 memancarkan log real-time:
  ```text
  [KERNEL] Entered kernel_main()
  [GDT] Initializing 32-bit GDT & reloading segment registers...
  [GDT] GDT loaded successfully (CS=0x08, DS=0x10)!
  [IDT] Initializing IDT 256 gates...
  [IDT] IDT loaded successfully via LIDT!
  [PIC] Remapping 8259 PIC (Master: 32, Slave: 40)...
  [PIC] PIC remapped successfully!
  [ISR] Registering CPU Exception & IRQ stubs...
  [ISR] ISR handlers registered successfully!
  [CPU] Enabling interrupts via STI...
  [CPU] Interrupts enabled safely!
  [KERNEL] Clearing VGA screen with blue background...
  [KERNEL] Screen cleared successfully!
  [KERNEL] Writing Phase 2 VGA banner...
  [TEST] Testing software interrupt INT 3...
  [ISR] INT 3 Breakpoint Exception Handler Intercepted Successfully!
  [KERNEL] Phase 2 initialization complete! Entering infinite loop...
  ```

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
