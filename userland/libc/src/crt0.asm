; crt0.asm - Userland C Runtime Entry Point Stub for Nebula OS
[BITS 32]

global _start
extern main
extern exit

section .text
_start:
    ; Clean frame pointer
    xor ebp, ebp

    ; Extract argc and argv from stack
    pop eax         ; argc
    mov ecx, esp    ; argv pointer

    push ecx        ; argv
    push eax        ; argc

    call main

    ; Exit process with return status of main()
    push eax
    call exit
    hlt
