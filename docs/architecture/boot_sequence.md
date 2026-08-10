# Nebula OS - Spesifikasi Alur Booting (Boot Sequence)

## 1. Ringkasan Alur Eksekusi Booting XNU-Hybrid

Alur booting Nebula OS memindahkan CPU dari status awal menyala (atau saat emulator QEMU dijalankan) menuju lingkungan eksekusi C++ XNU-Hybrid Architecture yang utuh.

```text
+-------------------------------------------------------+
|  1. Firmware / BIOS / Multiboot Bootloader            |
|     Memuat binary ELF bootable ke RAM di 1MB (0x100000) |
+-------------------------------------------------------+
                           |
                           v
+-------------------------------------------------------+
|  2. Entry Point Assembly (`boot/x86_64/boot.asm`)     |
|     - Verifikasi Magic Number Multiboot               |
|     - Menyiapkan Stack Pointer 32-bit (16 KiB)        |
|     - Far Jump ke kernel_main() Entry Point           |
+-------------------------------------------------------+
                           |
                           v
+-------------------------------------------------------+
|  3. XNU Subsystems Init (`kernel/core/kernel.cpp`)    |
|     - Serial Debug Logging (`iokit/serial/`)           |
|     - Pexpert Device Tree Init (`pexpert/`)           |
|     - Libkern OSObject & Utility Init (`libkern/`)    |
|     - Libsa Kext Loader Subsystem Init (`libsa/`)     |
|     - Security MAC Credentials Framework (`security/`)|
|     - San Kernel Memory Sanitizers (`san/`)           |
+-------------------------------------------------------+
                           |
                           v
+-------------------------------------------------------+
|  4. Mach Core & BSD Services Layer Init               |
|     - GDT, IDT Vector Interupsi, & PIC Remap (`mach/`)|
|     - Engine Memori PMM, VMM Paging, & Heap (`mach/`) |
|     - Preemptive Multitasking Scheduler (`mach/`)     |
|     - VFS, Initrd RAM Disk, & Syscall Gate (`bsd/`)   |
|     - Drivers Bochs BGA, Mouse IRQ12, PIT (`iokit/`)  |
|     - Compositor Desktop GUI Engine (`gui/`)          |
+-------------------------------------------------------+
```

---

## 2. Rincian Cronologis Booting Kernel

### Fase 1: Serah Terima dari Bootloader
Bootloader standar Multiboot (GRUB atau QEMU `-kernel`) memuat file `nebula.elf` ke memori fisik pada alamat `0x100000` (1 MiB).
Bootloader mengirimkan parameter:
* Register `EAX`: Magic number `0x1BADB002` (Multiboot 1).
* Register `EBX`: Alamat pointer ke struktur Informasi Multiboot di RAM.

### Fase 2: Pengujian Kemampuan CPU & Stack Setup
File `boot.asm` melakukan penyiapan lingkungan eksekusi awal:
1. **Pemeriksaan Multiboot**: Memastikan register `EAX` berisi magic number yang valid.
2. **Penyiapan Stack**: Mengalokasikan 16 KiB stack terisolasi di memori BSS dan melakukan *alignment* 16-byte System V ABI.
3. **Panggilan Kernel Entry Point**: Mendorong argumen `magic` dan `mb_info` ke stack, lalu memanggil `kernel_main`.

### Fase 3: Inisialisasi Modul XNU Subsystems
Saat memasuki `kernel_main()` pada [kernel/core/kernel.cpp](file:///c:/project/Nebula/kernel/core/kernel.cpp):
1. **UART Serial Logger (`iokit/serial/`)**: Menyiapkan komunikasi port COM1 untuk memancarkan log booting hardware.
2. **Platform Expert (`pexpert/`)**: Menyiapkan pengenal *device tree* awal hardware.
3. **Libkern (`libkern/`)**: Inisialisasi engine C++ object refcounting (`OSObject`).
4. **Libsa (`libsa/`)**: Inisialisasi subsystem *standalone loader* extension.
5. **Security Framework (`security/`)**: Menyiapkan *Mandatory Access Control (MAC)* dan kredensial pengguna (`ucred_t`).
6. **Sanitizers (`san/`)**: Mengaktifkan kait pelacak integritas memori kernel.

### Fase 4: Eksekusi Layer Mach Core & BSD Services
1. **GDT & IDT Reload (`mach/arch/`)**: Memuat 256 Vector Interupsi IDT dan memetakan ulang 8259 PIC (IRQ 0-15 ke Vector 32-47).
2. **PMM & VMM Paging (`mach/vm/`)**: Mengaktifkan pemetaan memori fisik bitmap allocator, VMM page tables (termasuk identity mapping VRAM `0xFD000000`), dan Kernel Heap pool `0x00C00000`.
3. **Preemptive Scheduler (`mach/sched/`)**: Mengaktifkan penjadwalan multitasking preemptif berbasis interupsi PIT 100Hz.
4. **Virtual File System & Syscall (`bsd/vfs/` & `bsd/sys/`)**: Meng-mount RAM Disk `/initrd/` dan memanggil pendaftaran IDT Vector 128 (`INT 0x80`).
5. **Bochs BGA Driver & Window Manager (`iokit/display/` & `gui/`)**: Memrogram hardware Bochs BGA Port `0x1CE`/`0x1CF` ke mode 800x600 32-bit BPP, menginisialisasi Compositor Desktop GUI, lalu membuka interupsi `sti`.
