# Spesifikasi Driver Perangkat Keras (Hardware Drivers)

## 1. Arsitektur Driver Modular
Driver dalam Nebula OS dikelola sebagai modul terpisah yang berinteraksi langsung dengan port I/O x86_64 (`inb`/`outb`, `inw`/`outw`) dan pemetaan memori perangkat (Memory-Mapped I/O / MMIO).

---

## 2. Driver Utama Phase 1 - 3

1. **VGA Text Mode & VBE Framebuffer**:
   * **VGA Text Mode**: Menulis ke buffer memori `0xB8000` (80x25 karakter 16-bit warna) pada tahap awal boot.
   * **VGA/VBE Linear Framebuffer**: Mengatur mode grafis tinggi (1024x768 32-bit RGBA color) untuk antarmuka GUI Window Manager.

2. **PS/2 Keyboard Driver**:
   * Mengolah interupsi IRQ 1 (Vector 33 di IDT).
   * Membaca scancode dari I/O Port `0x60`, menerjemahkannya ke karakter ASCII / Virtual Key Event, lalu memasukkannya ke ring buffer keyboard.

3. **PIT (Programmable Interval Timer)**:
   * Mengolah interupsi IRQ 0 (Vector 32 di IDT).
   * Dikonfigurasi pada frekuensi 100 Hz (10 milidetik per tick) untuk mendukung penakaran waktu OS dan penjadwalan multitasking preemptif.

4. **Serial Controller (COM1 / UART 16550)**:
   * Mengirimkan log teks kernel ke Port I/O `0x3F8`.
   * Memungkinkan pencetakan log debugging secara real-time ke terminal QEMU (`-serial stdio`).
