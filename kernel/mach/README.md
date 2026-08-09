# Mach Microkernel Core Implementation

Folder `kernel/mach/` berisi implementasi subsistem dasar dari Mach Microkernel Core.

## Source Tree

```text
kernel/mach/
├── arch/        # Implementasi Abstraksi CPU (GDT, IDT, PIC, TSS, ISR Assembly)
├── vm/          # Implementasi Manajemen Memori (PMM Bitmap, VMM Paging, Heap)
└── sched/       # Implementasi Scheduler Multitasking Preemptif
```

## Sub-Direktori

* **`arch/`**: Implementasi GDT, IDT, PIC, TSS, dan ISR assembly stubs.
* **`vm/`**: Implementasi PMM bitmap, VMM page tables, dan overloading `new`/`delete`.
* **`sched/`**: Implementasi algoritma Round-Robin scheduling dan context switching.
