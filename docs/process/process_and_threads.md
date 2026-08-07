# Spesifikasi Proses dan Thread Nebula OS

## 1. Konsep Dasar
Nebula OS mendukung alur eksekusi multitasking dengan pemisahan tegas antara **Proses** (unit alokasi sumber daya & isolasi memori) dan **Thread** (unit eksekusi penjadwalan).

---

## 2. Struktur Data Utama

### 2.1 Process Control Block (PCB)
PCB menyimpan status lengkap dari sebuah proses:
* `pid`: Process ID unik.
* `pml4_phys`: Alamat fisik PML4 Page Table proses (Isolasi Memori Virtual).
* `state`: Status proses (`PROCESS_CREATED`, `PROCESS_READY`, `PROCESS_RUNNING`, `PROCESS_BLOCKED`, `PROCESS_TERMINATED`).
* `threads`: List / Vector thread yang dimiliki oleh proses ini.
* `file_descriptors`: Tabel handle file terbuka (VFS File Descriptor Table).

### 2.2 Thread Control Block (TCB)
TCB menyimpan konteks registrasi CPU untuk eksekusi individu:
* `tid`: Thread ID unik.
* `parent_process`: Pointer ke PCB pemilik.
* `rsp`: Stack Pointer 64-bit saat context switch terjadi.
* `kernel_stack`: Alamat batas atas Stack Kernel (diperlukan saat interupsi/syscall berpindah dari Ring 3 ke Ring 0).
* `cpu_context`: Register CPU (`RAX`, `RBX`, `RCX`, `RDX`, `RSI`, `RDI`, `RBP`, `R8`-`R15`, `RIP`, `RFLAGS`).

---

## 3. Siklus Hidup Proses (Process Lifecycle)

```
[Create Process] ---> READY <---> RUNNING ---> TERMINATED (Zombie / Reaped)
                       ^            |
                       |            v
                       +--- BLOCKED (Menunggu I/O / Timer / Event)
```

1. **Creation**: `process_create()` mengalokasikan PCB, tabel PML4 baru, dan stack awal.
2. **Execution**: Scheduler memilih thread dari antrean `READY` dan melakukan context switch.
3. **Blocking**: Thread berpindah ke status `BLOCKED` jika menunggu pembacaan disk, input keyboard, atau timer.
4. **Termination**: Saat fungsi `exit()` dipanggil, sumber daya memori dibebaskan dan status berubah menjadi `TERMINATED`.
