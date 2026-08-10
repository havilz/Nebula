# Mach Scheduler & Process Headers

Folder `include/mach/sched/` berisi deklarasi header untuk manajemen proses, thread, dan scheduler.

## Source Tree

```text
include/mach/sched/
├── process.hpp   # Process Control Block (PCB) & Thread Control Block (TCB) Header
└── scheduler.hpp # Preemptive Round-Robin Scheduler Header
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `process.hpp` | Deklarasi struktur PCB, TCB, register CPU, dan status thread. |
| `scheduler.hpp` | Deklarasi kelas Scheduler preemptif 100Hz time-slicing dan alokasi thread. |
