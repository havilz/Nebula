; ISR Exception & IRQ Hardware Stubs for x86 32-bit Protected Mode
[BITS 32]

extern isr_handler
extern irq_handler

%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push dword 0    ; Push dummy error code
    push dword %1   ; Push interrupt vector number
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    ; Error code is automatically pushed by CPU
    push dword %1   ; Push interrupt vector number
    jmp isr_common_stub
%endmacro

%macro IRQ 2
global irq%1
irq%1:
    cli
    push dword 0    ; Push dummy error code
    push dword %2   ; Push mapped IDT vector number
    jmp irq_common_stub
%endmacro

section .text

; 32 CPU Exception Stubs
ISR_NOERRCODE 0   ; 0: Divide By Zero Exception
ISR_NOERRCODE 1   ; 1: Debug Exception
ISR_NOERRCODE 2   ; 2: Non Maskable Interrupt Exception
ISR_NOERRCODE 3   ; 3: Breakpoint Exception
ISR_NOERRCODE 4   ; 4: Into Detected Overflow Exception
ISR_NOERRCODE 5   ; 5: Out of Bounds Exception
ISR_NOERRCODE 6   ; 6: Invalid Opcode Exception
ISR_NOERRCODE 7   ; 7: No Coprocessor Exception
ISR_ERRCODE   8   ; 8: Double Fault Exception
ISR_NOERRCODE 9   ; 9: Coprocessor Segment Overrun Exception
ISR_ERRCODE   10  ; 10: Bad TSS Exception
ISR_ERRCODE   11  ; 11: Segment Not Present Exception
ISR_ERRCODE   12  ; 12: Stack Fault Exception
ISR_ERRCODE   13  ; 13: General Protection Fault Exception (#GP)
ISR_ERRCODE   14  ; 14: Page Fault Exception (#PF)
ISR_NOERRCODE 15  ; 15: Unknown Interrupt Exception
ISR_NOERRCODE 16  ; 16: Coprocessor Fault Exception
ISR_NOERRCODE 17  ; 17: Alignment Check Exception
ISR_NOERRCODE 18  ; 18: Machine Check Exception
ISR_NOERRCODE 19  ; 19: SIMD Floating Point Exception
ISR_NOERRCODE 20  ; 20: Virtualization Exception
ISR_NOERRCODE 21  ; 21: Reserved
ISR_NOERRCODE 22  ; 22: Reserved
ISR_NOERRCODE 23  ; 23: Reserved
ISR_NOERRCODE 24  ; 24: Reserved
ISR_NOERRCODE 25  ; 25: Reserved
ISR_NOERRCODE 26  ; 26: Reserved
ISR_NOERRCODE 27  ; 27: Reserved
ISR_NOERRCODE 28  ; 28: Reserved
ISR_NOERRCODE 29  ; 29: Reserved
ISR_ERRCODE   30  ; 30: Security Exception
ISR_NOERRCODE 31  ; 31: Reserved

; 16 IRQ Hardware Stubs (Mapped to IDT Vectors 32-47)
IRQ 0, 32   ; IRQ0: PIT Timer
IRQ 1, 33   ; IRQ1: PS/2 Keyboard
IRQ 2, 34   ; IRQ2: Cascade for 8259 Slave PIC
IRQ 3, 35   ; IRQ3: Serial Port COM2/4
IRQ 4, 36   ; IRQ4: Serial Port COM1/3
IRQ 5, 37   ; IRQ5: LPT2 / Sound Card
IRQ 6, 38   ; IRQ6: Floppy Disk Controller
IRQ 7, 39   ; IRQ7: LPT1 / Spurious
IRQ 8, 40   ; IRQ8: Real Time Clock (RTC)
IRQ 9, 41   ; IRQ9: ACPI
IRQ 10, 42  ; IRQ10: Available
IRQ 11, 43  ; IRQ11: Available
IRQ 12, 44  ; IRQ12: PS/2 Mouse
IRQ 13, 45  ; IRQ13: FPU / Coprocessor
IRQ 14, 46  ; IRQ14: Primary ATA Controller
IRQ 15, 47  ; IRQ15: Secondary ATA Controller

isr_common_stub:
    pusha           ; Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax
    mov ax, ds      ; Lower 16 bits of eax = ds
    push eax        ; Save original data segment descriptor

    mov ax, 0x10    ; Load Kernel Data Segment (0x10)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp        ; Pass pointer to registers_t struct as argument
    call isr_handler
    add esp, 4      ; Clean argument pointer off stack

    pop eax         ; Restore original data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa            ; Restores edi, esi, ebp, esp, ebx, edx, ecx, eax
    add esp, 8      ; Cleans up pushed int_no and err_code
    iret            ; Interrupt return

irq_common_stub:
    pusha           ; Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax
    mov ax, ds      ; Lower 16 bits of eax = ds
    push eax        ; Save original data segment descriptor

    mov ax, 0x10    ; Load Kernel Data Segment (0x10)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp        ; Pass pointer to registers_t struct as argument
    call irq_handler
    add esp, 4      ; Clean argument pointer off stack

    pop eax         ; Restore original data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa            ; Restores edi, esi, ebp, esp, ebx, edx, ecx, eax
    add esp, 8      ; Cleans up pushed int_no and err_code
    iret            ; Interrupt return
