# Spesifikasi Kebijakan Penjadwalan (Scheduling Policies)

## 1. Algoritma Penjadwalan
Scheduler Nebula OS mendukung beberapa algoritma penjadwalan multitasking preemptif:

1. **Round-Robin (Default Initial Policy)**:
   * Setiap thread mendapatkan jatah waktu eksekusi (*Time Slice / Quantum*) yang sama (misalnya 10 milidetik).
   * Saat quantum habis, interupsi timer hardware memicu penukaran konteks ke thread berikutnya dalam antrean circular.

2. **Multilevel Feedback Queue (MLFQ)**:
   * Memiliki beberapa tingkat antrean prioritas (Priority Levels 0 - 3).
   * Tugas-tugas I/O-bound (seperti input Keyboard/Mouse) diprioritaskan tinggi agar antarmuka responsif.
   * Tugas-tugas CPU-bound (komputasi berat) dipindahkan secara bertahap ke antrean prioritas yang lebih rendah.

---

## 2. Mekanisme Context Switching x86_64

Saat interupsi timer terjadi (PIT / APIC Timer):
1. Hardware CPU menyimpan `SS`, `RSP`, `RFLAGS`, `CS`, `RIP` ke dalam Kernel Stack.
2. Handler Assembly (`interrupt_stub`) menyimpan register tersisa (`RAX`, `RBX`, `RCX`, `RDX`, `RSI`, `RDI`, `RBP`, `R8`-`R15`).
3. Scheduler memilih TCB berikutnya yang berstatus `READY`.
4. Register `RSP` diganti ke stack milik thread baru.
5. Assembly memulihkan register thread baru dan mengeksekusi instruksi `iretq` / `sysret`.
