# Spesifikasi Kebijakan Penjadwalan Scheduler Nebula OS (Mach Sched Layer)

## 1. Gambaran Umum
Penjadwalan thread pada Nebula OS diimplementasikan di [scheduler.cpp](file:///c:/project/Nebula/kernel/mach/sched/scheduler.cpp) menggunakan algoritma **Preemptive Round-Robin Multi-Threading**.

---

## 2. Parameter Kebijakan Penjadwalan

* **Algoritma**: Preemptive Round-Robin (Time Slicing).
* **Frekuensi Tick**: 100 Hz (Interupsi IRQ 0 PIT Timer setiap 10 milidetik).
* **Kapasitas Maksimal Thread**: `MAX_THREADS = 16`.
* **Ukuran Stack Per Thread**: 16 KiB terisolasi via alokasi memori `kmalloc()`.

---

## 3. Alur Kerja Context Switching (Time Slice)

1. Interupsi PIT Timer (IRQ 0) memicu penangan interupsi C++ `Scheduler::handle_timer_tick(regs)`.
2. Scheduler memperbarui pointer `regs` pada TCB thread yang sedang berjalan (`RUNNING` -> `READY`).
3. Scheduler melompati thread indeks berikutnya dalam antrean yang berstatus `READY`.
4. Scheduler mengubah status thread target menjadi `RUNNING` dan mengembalikan pointer `regs` miliknya.
5. Stub assembly `irq_common_stub` pada [isr.asm](file:///c:/project/Nebula/kernel/mach/arch/isr.asm) mengeksekusi penukaran register `ESP` ke stack thread target.
