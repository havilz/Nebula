# Makefile untuk Nebula OS - Phase 2 (GDT, IDT, PIC & Interrupt Handling)
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
           -fno-leading-underscore -fno-asynchronous-unwind-tables -fno-unwind-tables \
           -fno-builtin -fno-tree-loop-distribute-patterns \
           -mno-sse -mno-sse2 -mno-mmx -mno-80387 -Iinclude
LDFLAGS  = -m elf_i386 --omagic -T boot/linker.ld
ASMFLAGS = -f elf32

# --- Debug build ---
DEBUG_DIR    = build_debug
CFLAGS_DEBUG = -m32 -ffreestanding -O0 -g -Wall -Wextra -fno-exceptions -fno-rtti \
               -fno-leading-underscore -fno-asynchronous-unwind-tables -fno-unwind-tables \
               -fno-builtin -fno-tree-loop-distribute-patterns \
               -mno-sse -mno-sse2 -mno-mmx -mno-80387 -Iinclude
ASMFLAGS_DEBUG = -f elf32 -g -F dwarf

OBJS = $(BUILD_DIR)/boot.o \
       $(BUILD_DIR)/isr.o \
       $(BUILD_DIR)/kernel.o

DEBUG_OBJS = $(DEBUG_DIR)/boot.o \
             $(DEBUG_DIR)/isr.o \
             $(DEBUG_DIR)/kernel.o

all: $(BUILD_DIR)/nebula.elf

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/boot.o: $(BOOT_DIR)/boot.asm | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILD_DIR)/isr.o: $(KERNEL_DIR)/arch/x86_64/interrupts/isr.asm | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILD_DIR)/kernel.o: $(KERNEL_DIR)/core/kernel.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $(BUILD_DIR)/kernel_pe.o
	$(OBJCOPY) -I pe-i386 -O elf32-i386 $(BUILD_DIR)/kernel_pe.o $@

$(BUILD_DIR)/nebula.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

run: all
	$(QEMU) -kernel $(BUILD_DIR)/nebula.elf

# --- Debug build & run targets ---
$(DEBUG_DIR):
	mkdir -p $(DEBUG_DIR)

$(DEBUG_DIR)/boot.o: $(BOOT_DIR)/boot.asm | $(DEBUG_DIR)
	$(ASM) $(ASMFLAGS_DEBUG) $< -o $@

$(DEBUG_DIR)/isr.o: $(KERNEL_DIR)/arch/x86_64/interrupts/isr.asm | $(DEBUG_DIR)
	$(ASM) $(ASMFLAGS_DEBUG) $< -o $@

$(DEBUG_DIR)/kernel.o: $(KERNEL_DIR)/core/kernel.cpp | $(DEBUG_DIR)
	$(CC) $(CFLAGS_DEBUG) -c $< -o $(DEBUG_DIR)/kernel_pe.o
	$(OBJCOPY) -I pe-i386 -O elf32-i386 $(DEBUG_DIR)/kernel_pe.o $@

$(DEBUG_DIR)/nebula.elf: $(DEBUG_OBJS)
	$(LD) $(LDFLAGS) -o $@ $(DEBUG_OBJS)

debug: $(DEBUG_DIR)/nebula.elf

qemu-debug: debug
	$(QEMU) -kernel $(DEBUG_DIR)/nebula.elf -s -S -serial stdio

clean:
	rm -rf $(BUILD_DIR) $(DEBUG_DIR) kernel_debug.log *.o ./-p

.PHONY: all run debug qemu-debug clean