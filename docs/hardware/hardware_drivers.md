# Spesifikasi Driver Perangkat Keras Nebula OS (IOKit Driver Framework)

## 1. Gambaran Umum
Seluruh driver perangkat keras Nebula OS mengadopsi IOKit Object-Oriented C++ Driver Framework (`include/iokit/` & `kernel/iokit/`) mewarisi base class abstrak `IODevice` (`include/iokit/iodevice.hpp`).

---

## 2. Rincian Driver IOKit Utama

### 2.1 Driver Display Bochs VBE BGA (`iokit/display/vbe.hpp` & `vbe.cpp`)
* Menangani pemrograman register I/O Bochs BGA Port `0x1CE` & `0x1CF`.
* Mengonfigurasi mode grafis resolusi tinggi 800x600 piksel 32-bit True Color (BPP).
* Menyediakan Linear Framebuffer (LFB) di `0xFD000000` dengan dukungan double buffering RAM.

### 2.2 Driver Input PS/2 Mouse & Keyboard (`iokit/input/`)
* **PS/2 Mouse** (`mouse.hpp` & `mouse.cpp`): Membaca paket interupsi IRQ 12 (Vector 44) dan menggerakkan Graphic Cursor di atas Compositor GUI Window Manager.
* **PS/2 Keyboard** (`keyboard.hpp` & `keyboard.cpp`): Membaca scancode interupsi IRQ 1 (Vector 33) dan mengonversinya ke tabel karakter US QWERTY.

### 2.3 Driver PIT Timer 8254 (`iokit/timer/pit.hpp` & `pit.cpp`)
* Memrogram 8254 Programmable Interval Timer pada Port `0x40`-`0x43` untuk memicu interupsi IRQ 0 (Vector 32) pada frekuensi 100 Hz.

### 2.4 Driver UART 16550 Serial Debugging (`iokit/serial/serial.hpp` & `serial.cpp`)
* Memrogram pengirim karakter serial pada Port COM1 (`0x3F8`) untuk memancarkan log debug kernel ke konsol terminal pengguna.
