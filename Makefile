# Makefile untuk Nebula OS - Phase 1 (Barebones Kernel)
# Dikompilasi menggunakan MSYS2 UCRT64 toolchain & LLD Linker di Windows

CC      = g++
ASM     = nasm
LD      = ld.lld
OBJCOPY = objcopy
QEMU    = qemu-system-i386

BUILD_DIR = build
BOOT_DIR  = boot/x86_64
KERNEL_DIR = kernel

CFLAGS   = -m32 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti \
           -fno-leading-underscore -fno-asynchronous-unwind-tables -fno-unwind-tables -Iinclude
LDFLAGS  = -m elf_i386 -T boot/linker.ld
ASMFLAGS = -f elf32

OBJS = $(BUILD_DIR)/boot.o \
       $(BUILD_DIR)/vga.o \
       $(BUILD_DIR)/kernel.o

all: $(BUILD_DIR)/nebula.elf

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/boot.o: $(BOOT_DIR)/boot.asm | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILD_DIR)/vga.o: $(KERNEL_DIR)/drivers/console/vga.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $(BUILD_DIR)/vga_pe.o
	$(OBJCOPY) -I pe-i386 -O elf32-i386 $(BUILD_DIR)/vga_pe.o $@

$(BUILD_DIR)/kernel.o: $(KERNEL_DIR)/core/kernel.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $(BUILD_DIR)/kernel_pe.o
	$(OBJCOPY) -I pe-i386 -O elf32-i386 $(BUILD_DIR)/kernel_pe.o $@

$(BUILD_DIR)/nebula.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

run: all
	$(QEMU) -kernel $(BUILD_DIR)/nebula.elf

clean:
	rm -rf $(BUILD_DIR) kernel_debug.log

.PHONY: all run clean
