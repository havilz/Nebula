# Mach Microkernel Core Headers

Folder `include/mach/` berisi deklarasi header untuk subsistem paling dasar dari microkernel core (CPU Hal, Memory Manager, dan Scheduler).

## Source Tree

```text
include/mach/
├── arch/        # Header Abstraksi CPU (GDT, IDT, PIC, TSS, ISR)
├── vm/          # Header Manajemen Memori (PMM Bitmap, VMM Paging, Heap)
└── sched/       # Header Multitasking Scheduler (PCB, TCB, Time Slicing)
```

## Sub-Direktori

* **`arch/`**: Mengelola descriptor CPU, tabel interupsi, dan switching mode.
* **`vm/`**: Mengelola memori fisik, pemetaan halaman virtual, dan alokasi heap.
* **`sched/`**: Mengelola struktur proses, thread, dan algoritma penjadwalan preemptif.
