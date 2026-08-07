# Architecture Decision Record (ADR 001): 64-bit Long Mode Higher-Half Kernel

## Status
Diterima (Accepted)

## Konteks
Saat merancang sistem operasi baru dari nol, terdapat dua keputusan arsitektur utama:
1. Memilih antara mode 32-bit (Protected Mode) vs 64-bit (Long Mode).
2. Memilih model tata letak memori virtual kernel (Direct Lower-Half vs Higher-Half Kernel).

## Keputusan
Nebula OS akan menggunakan arsitektur **64-bit Native (x86_64 Long Mode)** dengan model **Higher-Half Kernel Layout** (di alamat virtual `0xFFFF800000000000`).

## Konsekuensi

### Keuntungan:
* **Performa Tinggi**: Mengakses register 64-bit (`RAX`, `RBX`, `R8`-`R15`) secara langsung dan menggunakan mekanisme cepat `SYSCALL`/`SYSRET`.
* **Isolasi Memori Sempurna**: Area memori Userland (`0x0` - `0x00007FFFFFFFFFFF`) terpisah secara penuh dari area Kernel (`0xFFFF800000000000` - `0xFFFFFFFFFFFFFFFF`).
* **System Call Tanpa Context Switch Page Table**: Kernel berada di Higher-Half dari setiap peta memori proses, sehingga peralihan Ring 3 ke Ring 0 tidak memerlukan *Page Directory Base Register (CR3) Reload*.

### Risiko & Tantangan:
* Memerlukan langkah booting bertahap dari 32-bit ke 64-bit (menyiapkan tabel PML4, mengaktifkan PAE & EFER Long Mode MSR).
* Tidak dapat menggunakan C++ Standard Library bawaan host compiler (wajib `-ffreestanding -fno-exceptions -fno-rtti`).
