; isr.asm - Interrupt Service Routine (ISR) Stubs for Nebula OS
[BITS 32]

extern cpp_interrupt_handler
global isr_stub_table

section .text

; ------------------------------------------------------------------------
; Common ISR entry point called by individual vector stubs
; ------------------------------------------------------------------------
isr_common_stub:
    pusha                                  ; Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax

    push esp                               ; Push pointer to InterruptRegisters struct
    call cpp_interrupt_handler
    add esp, 4                             ; Clean up pushed pointer

    popa                                   ; Restores edi, esi, ebp, esp, ebx, edx, ecx, eax
    add esp, 8                             ; Clean up int_no and err_code
    iretd

; ------------------------------------------------------------------------
; Macro for ISR stubs without hardware error code (pushes dummy 0)
; ------------------------------------------------------------------------
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push dword 0                           ; Dummy error code
    push dword %1                          ; Vector number
    jmp isr_common_stub
%endmacro

; ------------------------------------------------------------------------
; Macro for ISR stubs with hardware error code
; ------------------------------------------------------------------------
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    push dword %1                          ; Vector number
    jmp isr_common_stub
%endmacro

; ------------------------------------------------------------------------
; CPU Exception Stubs (Vectors 0 - 31)
; ------------------------------------------------------------------------
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_ERRCODE   30
ISR_NOERRCODE 31

; ------------------------------------------------------------------------
; Hardware IRQ Stubs (Vectors 32 - 47)
; ------------------------------------------------------------------------
ISR_NOERRCODE 32
ISR_NOERRCODE 33
ISR_NOERRCODE 34
ISR_NOERRCODE 35
ISR_NOERRCODE 36
ISR_NOERRCODE 37
ISR_NOERRCODE 38
ISR_NOERRCODE 39
ISR_NOERRCODE 40
ISR_NOERRCODE 41
ISR_NOERRCODE 42
ISR_NOERRCODE 43
ISR_NOERRCODE 44
ISR_NOERRCODE 45
ISR_NOERRCODE 46
ISR_NOERRCODE 47

; ------------------------------------------------------------------------
; ISR Stub Address Table for IDT Initialization
; ------------------------------------------------------------------------
section .data
align 4
isr_stub_table:
    dd isr0,  isr1,  isr2,  isr3,  isr4,  isr5,  isr6,  isr7
    dd isr8,  isr9,  isr10, isr11, isr12, isr13, isr14, isr15
    dd isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23
    dd isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31
    dd isr32, isr33, isr34, isr35, isr36, isr37, isr38, isr39
    dd isr40, isr41, isr42, isr43, isr44, isr45, isr46, isr47
