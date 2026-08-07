# CHANGELOG & DOKUMENTASI TEKNIS - NEBULA OS

Dokumen ini mencatat seluruh riwayat perubahan, keputusan arsitektur teknis, diagnosa masalah, dan solusi yang diterapkan pada proyek **Nebula OS**. Dokumen ini diperbarui secara berkala setiap kali suatu Fase / Task selesai diimplementasikan.

---

## [Task 5] - Task State Segment (TSS 64-bit), PCB/TCB, & Preemptive Round-Robin Scheduler

### Ringkasan Tujuan
Membangun subsistem Multitasking dan Scheduler Preemptif: Task State Segment (TSS 64-bit 104 byte), struktur data Process Control Block (PCB) & Thread Control Block (TCB), serta algoritma Preemptive Round-Robin Time Slicing berbasis timer IRQ 0 (100 Hz).

---

### Perubahan & Komponen Utama yang Dibuat

#### 1. Header Abstraksi PML4 Paging 64-bit (`include/kernel/arch/x86_64/paging64.hpp`)
- **Tabel Paging 4-Level**: Struct `pml4_table_t`, `pdpt_table_t`, `pd_table64_t`, dan flag proteksi 64-bit (`PAGE64_PRESENT`, `PAGE64_WRITABLE`, `PAGE64_USER`, `PAGE64_HUGE`).

#### 2. Task State Segment (TSS 64-bit) (`include/kernel/arch/x86_64/tss.hpp` & `kernel/arch/x86_64/tss/tss.cpp`)
- **Struktur TSS**: Struct `TSSEntry` (104 byte) menyimpan pointer stack kernel `rsp0` dan `ist[7]` (Interrupt Stack Table) untuk transisi terisolasi saat interupsi/syscall.
- **TSS Manager**: `TSS::init(kernel_stack)` dan `TSS::set_kernel_stack(stack_ptr)`.

#### 3. Process Control Block (PCB) & Thread Control Block (TCB) (`include/kernel/process/process.hpp`)
- **Status Thread**: Enum class `ThreadState` (`EMBRYO`, `READY`, `RUNNING`, `WAITING`, `TERMINATED`).
- **Register Context**: Struct `cpu_context_t` menyimpan register CPU (`EAX`, `EBX`, `ECX`, `EDX`, `ESI`, `EDI`, `EBP`, `ESP`, `EIP`, `EFLAGS`).
- **Struktur Thread & Process**: Struct `Thread` (TID, name, stack, context) dan `Process` (PID, page directory, threads).

#### 4. Preemptive Round-Robin Scheduler (`include/kernel/scheduler/scheduler.hpp` & `kernel/scheduler/scheduler.cpp`)
- **Timer Tick Dispatcher**: `Scheduler::handle_timer_tick(regs)` dipanggil oleh IRQ 0 setiap 10ms. Scheduler menyimpan konteks thread aktif, memilih thread `READY` berikutnya via Round-Robin, dan memperbarui register CPU frame `regs`.
- **Manajemen Kernel Thread**: `Scheduler::create_kernel_thread(entry, name)` mengalokasikan stack 16 KiB per thread di Kernel Heap.

#### 5. Pengujian Concurrent Execution Kernel Thread Alpha & Beta di Kernel Main (`kernel/core/kernel.cpp`)
- **Thread Alpha & Beta**: Dua kernel thread berjalan secara independen di latar belakang, memancarkan log real-time ke Serial COM1 dan memperbarui indikator VGA Console secara bergantian.

---

### Catatan Diagnostik & Pemecahan Masalah Teknis

1. **Fix Exception `#UD` (Invalid Opcode) pada Mode Debug (MinGW `-O1 -g`)**:
   - *Penyebab*: Pembuatan thread baru pada `create_kernel_thread()` belum memasukkan struktur `registers_t` (frame interupsi) pada bagian atas stack thread baru. Pada kompilasi debug `-O1 -g`, penukaran `esp` ke stack tanpa frame `registers_t` menyebabkan `iret` membaca memori acak sebagai `EIP` dan memicu exception `#UD`.
   - *Solusi*: Menginisialisasi `registers_t` frame (`CS=0x08`, `DS=0x10`, `EFLAGS=0x202`, `EIP=entry_point`) secara eksplisit di bagian atas stack thread baru, sehingga `iret` kembali secara aman dan bersih pada mode Release maupun Debug.

---

### Hasil Akhir Eksekusi (Verification Status)

- **Mode GUI (`mingw32-make run`)**: Jendela QEMU menampilkan spanduk konsol Phase 5:
  ```text
  [OK] 64-bit TSS & Kernel Stack Initialized
  [OK] Preemptive Scheduler Active (100 Hz Time Slicing)
  [OK] Thread Alpha & Thread Beta Running Concurrently!
  nebula> _
  ```
- **Mode Serial Terminal**: Port COM1 memancarkan log penukaran konteks real-time:
  ```text
  [KERNEL] Entered kernel_main()
  [TSS] Initializing Task State Segment (TSS 64-bit)...
  [SCHEDULER] Initializing Preemptive Round-Robin Scheduler...
  [SCHEDULER] Creating Kernel Thread Alpha...
  [SCHEDULER] Creating Kernel Thread Beta...
  [CPU] Enabling interrupts via STI...
  [KERNEL] Phase 5 Preemptive Multitasking active!
  [THREAD ALPHA] Running in background (TID 1)
  [THREAD BETA] Running in background (TID 2)
  [THREAD ALPHA] Running in background (TID 1)
  [THREAD BETA] Running in background (TID 2)
  ```

---

### Status Pengembangan Saat Ini:
- **Phase 1 (Multiboot 1 Kernel & VGA Driver)**: SELESAI (100%)
- **Phase 2 (GDT, IDT, PIC & ISR Handling)**: SELESAI (100%)
- **Phase 3 (Manajemen Memori - PMM, VMM Paging, & Heap)**: SELESAI (100%)
- **Phase 4 (Driver Hardware Input & Timer - PIT, PS/2, Serial)**: SELESAI (100%)
- **Phase 5 (Multitasking & Preemptive Scheduler)**: SELESAI (100%)
- **Phase 6 (Virtual File System & Ring 3 Syscall)**: Siap Dilanjutkan
