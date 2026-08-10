# Mach Preemptive Scheduler Implementation

Folder `kernel/mach/sched/` berisi kode sumber penjadwalan multitasking preemptif.

## Source Tree

```text
kernel/mach/sched/
└── scheduler.cpp # Implementasi Preemptive Round-Robin Scheduler
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `scheduler.cpp` | Manajemen antrean thread `READY`, pemotongan time-slice 100Hz, dan pertukaran register `ESP`. |
