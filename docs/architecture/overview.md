# Nebula OS - Gambaran Umum Arsitektur Sistem

## 1. Visi & Tujuan Utama
Nebula OS adalah sistem operasi 64-bit (x86_64) modular yang dibangun dari nol menggunakan C++ dan Assembly. Sistem ini dirancang mengadopsi arsitektur **XNU-Hybrid Kernel (1-to-1 Model)** yang memisahkan subsistem dasar secara sangat bersih: Mach Microkernel Core, BSD POSIX OS Services, dan IOKit C++ Driver Framework.

---

## 2. Paradigma Arsitektur XNU-Hybrid
Nebula OS mengadopsi struktur modul XNU-Hybrid:

* **Mach Microkernel Core Layer (`include/mach/` & `kernel/mach/`)**:
  * Abstraksi CPU (GDT, IDT, PIC, TSS 64-bit, ISR Stubs Assembly)
  * Manajemen Memori Fisik & Virtual (PMM Bitmap Allocator, VMM Page Tables, Heap Allocator)
  * Preemptive Scheduler & Threads (Round-Robin Context Switching, PCB, TCB)
  * IPC Communication Ports & Messages

* **BSD POSIX OS Services Layer (`include/bsd/` & `kernel/bsd/`)**:
  * Virtual File System (Abstraksi VFS & Initrd RAM Disk)
  * Process Management & Credentials (`ucred_t`)
  * System Call Dispatcher Vector 128 (`INT 0x80` & `SYSCALL`/`SYSRET`)
  * BSD Sockets & Networking Abstraction

* **IOKit Object-Oriented C++ Driver Framework (`include/iokit/` & `kernel/iokit/`)**:
  * Base Class `IODevice` dengan lifecycle management (`init`, `start`, `stop`)
  * Driver Display (Bochs BGA VBE Linear Framebuffer 800x600 32-bit BPP)
  * Driver Input (PS/2 Mouse IRQ 12 & PS/2 Keyboard)
  * Driver Timer (PIT 8254 100Hz) & UART 16550 Serial Debugging (COM1)

* **Kernel Support Modules (`libkern`, `libsa`, `security`, `pexpert`, `san`)**:
  * `libkern/`: C++ Utility Library & `OSObject` Refcounting System
  * `libsa/`: Standalone Kernel Runtime & Dynamic Kext Module Loader
  * `security/`: Mandatory Access Control (MAC Framework & Sandboxing)
  * `pexpert/`: Platform Expert Device Tree Parser & Machine Init
  * `san/`: Kernel Memory Sanitizer Hooks (KASAN / UBSAN)

* **Area Pengguna / Userland (Ring 3)**:
  * Library Standar C (`libnebula` / `libc`)
  * Process Init (PID 1) & Command Shell (`nebula-shell`)
  * Compositor Desktop GUI Engine & Window Manager (`include/gui/` & `kernel/gui/`)

---

## 3. Tata Letak Sistem & Peta Memori x86_64

```text
+-----------------------------------+ 0xFFFFFFFFFFFFFFFF (Batas Atas Virtual Address Space)
|                                   |
|       Higher-Half Kernel Space    | (Pemetaan langsung memori fisik & heap kernel)
|         0xFFFF800000000000        |
+-----------------------------------+ 0xFFFF800000000000
|       Unmapped Canonical Hole     | (Area tak terpetakan)
+-----------------------------------+ 0x00007FFFFFFFFFFF
|                                   |
|         Userland Space            | (Kode Proses, Stack, & Heap Aplikasi Ring 3)
|                                   |
+-----------------------------------+ 0x0000000000000000
```

---

## 4. Rincian Subsistem Utama XNU-Hybrid

### 4.1 Mach Microkernel Core (`kernel/mach/`)
* **`arch/`**: Membungkus instruksi assembly spesifik CPU, register CPU, GDT, IDT, TSS 64-bit, PIC 8259 remap, dan stub ISR context switching.
* **`vm/`**: PMM Bitmap Allocator, VMM Page Directory & Table Identity Paging, serta Kernel Heap Manager (`kmalloc`/`kfree`, `new`/`delete`).
* **`sched/`**: Preemptive Thread Scheduler, Process Control Block (PCB), Thread Control Block (TCB), dan time-slicing context switch.

### 4.2 BSD OS Services Layer (`kernel/bsd/`)
* **`vfs/`**: Virtual File System abstraction (`vnode`), file operation tables, dan Initrd RAM Disk driver (`/initrd/`).
* **`sys/`**: System Call Gate Vector 128 (`INT 0x80`) yang memetakan panggilan Ring 3 Userland ke layanan Ring 0 Kernel.

### 4.3 IOKit Driver Architecture (`kernel/iokit/`)
* Class C++ berorientasi objek yang mewarisi base class `IODevice` untuk driver Display (VBE Bochs BGA), Input (Mouse & Keyboard), Timer (PIT), dan Serial logging (COM1).

### 4.4 Kernel Runtime & Security Modules
* **`libkern/`**: C++ base class `OSObject` dengan `retain()` / `release()` atomic refcounting dan Port I/O `io.hpp`.
* **`libsa/`**: Standalone runtime dan dynamic module loader untuk kernel extension (kext).
* **`security/`**: Framework keamanan Mandatory Access Control (MAC) dan struktur kredensial pengguna (`ucred_t`).
* **`pexpert/`**: Platform Expert device tree parser dan hardware discovery.
* **`san/`**: Hooks pelacak kebocoran dan kerusakan memori kernel (KASAN/UBSAN).

---

## 5. Standar Kode & Panduan C++
* **Freestanding C++**: Tanpa C++ Standard Library bawaan OS (`<iostream>`, `<vector>`, `<string>` dilarang di Ring 0 sampai diimplementasikan di `libc` internal kita).
* **Tanpa Exception & RTTI**: Dikompilasi dengan `-fno-exceptions -fno-rtti` demi determinisme dan performa maksimal.
* **Manajemen Memori Eksplisit**: Semua alokasi memori wajib dibebaskan secara eksplisit atau dikelola menggunakan abstraksi RAII custom.
