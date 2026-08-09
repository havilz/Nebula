# IOKit Driver Framework Headers

Folder `include/iokit/` berisi deklarasi header untuk antarmuka driver C++ berorientasi objek (*Object-Oriented Driver Architecture*).

## Source Tree

```text
include/iokit/
├── iodevice.hpp  # Base Class Abstract IODevice Header
├── console/      # VGA Text Console Driver Header
├── display/      # Bochs BGA VBE Framebuffer Driver Header
├── input/        # PS/2 Mouse & Keyboard Drivers Headers
├── storage/      # ATA Primary Controller & MBR Parser Headers
├── timer/        # PIT 8254 Timer Driver Header
└── serial/       # UART 16550 Serial Debug Driver Header
```

## Sub-Direktori

* **`iodevice.hpp`**: Base class `IODevice` dengan metode virtual `init()`, `start()`, `stop()`.
* **`console/`**, **`display/`**, **`input/`**, **`storage/`**, **`timer/`**, **`serial/`**: Header driver perangkat keras modular.
