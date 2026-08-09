# Spesifikasi Manajemen Proses & Thread Nebula OS (Mach Sched Layer)

## 1. Gambaran Umum
Manajemen proses dan thread Nebula OS dikelola pada Mach Microkernel Core Layer (`include/mach/sched/` & `kernel/mach/sched/`) berbasis struktur data Process Control Block (PCB) dan Thread Control Block (TCB).

---

## 2. Struktur Data Utama

### 2.1 Process Control Block (PCB) (`process.hpp`)
Mewakili satu proses terisolasi yang memiliki ruang alamat virtual dan tabel kredensial sendiri:
* `pid_t pid`: ID proses unik.
* `uintptr_t page_directory`: Pointer fisik ke tabel paging (CR3) milik proses.
* `Thread* threads`: Daftar thread yang berjalan di dalam proses ini.

### 2.2 Thread Control Block (TCB)
Mewakili unit eksekusi terkecil yang dijadwalkan oleh Preemptive Scheduler:
* `uint32_t tid`: ID thread unik.
* `char name[32]`: Nama deskriptif thread (contoh: `"KernelMain"`, `"Thread Alpha"`).
* `ThreadState state`: Status thread (`EMBRYO`, `READY`, `RUNNING`, `BLOCKED`, `TERMINATED`).
* `uintptr_t stack_base` & `stack_size`: Alokasi memori stack terisolasi 16 KiB.
* `cpu_registers_t regs`: Simpanan konteks register CPU (`EAX`, `EBX`, `ECX`, `EDX`, `ESP`, `EBP`, `EIP`, `EFLAGS`).

---

## 3. Preemptive Context Switch

1. Saat interupsi timer (IRQ 0 PIT 100Hz) dipicu, CPU menyimpan konteks register thread ke stack.
2. `Scheduler::handle_timer_tick(regs)` memilih thread berikutnya dari antrean `READY`.
3. Fungsi `irq_handler` di [interrupts.cpp](file:///c:/project/Nebula/kernel/mach/arch/interrupts.cpp) mengembalikan pointer `ESP` baru milik thread target.
4. Instruksi assembly `mov esp, eax` pada [isr.asm](file:///c:/project/Nebula/kernel/mach/arch/isr.asm) beralih ke stack terisolasi thread target.
