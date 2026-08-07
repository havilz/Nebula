; vga.asm - Hardware VGA Text Driver for Nebula OS (32-bit ELF)
[BITS 32]

global vga_clear
global vga_write_at

section .text

; ------------------------------------------------------------------------
; helper function: update_cursor(x, y)
; ------------------------------------------------------------------------
update_cursor:
    push ebp
    mov ebp, esp
    push eax
    push edx
    push ebx

    mov eax, [ebp + 12]                    ; y
    imul eax, 80                           ; y * 80
    add eax, [ebp + 8]                     ; y * 80 + x
    mov ebx, eax

    ; send low byte to port 0x3D4/0x3D5
    mov dx, 0x3D4
    mov al, 0x0F
    out dx, al
    mov dx, 0x3D5
    mov al, bl
    out dx, al

    ; send high byte to port 0x3D4/0x3D5
    mov dx, 0x3D4
    mov al, 0x0E
    out dx, al
    mov dx, 0x3D5
    mov al, bh
    out dx, al

    pop ebx
    pop edx
    pop eax
    pop ebp
    ret

; ------------------------------------------------------------------------
; void vga_clear(uint8_t bg)
; [esp + 4] = bg color
; ------------------------------------------------------------------------
vga_clear:
    push ebp
    mov ebp, esp
    push edi
    push eax
    push ecx

    mov eax, [ebp + 8]                     ; bg color
    and eax, 0x0F
    shl eax, 4
    or eax, 0x0F                           ; white text on bg background
    shl eax, 8
    mov al, ' '                            ; space character

    mov edi, 0xB8000
    mov ecx, 2000
    rep stosw

    ; update cursor to (0,0)
    push dword 0
    push dword 0
    call update_cursor
    add esp, 8

    pop ecx
    pop eax
    pop edi
    pop ebp
    ret

; ------------------------------------------------------------------------
; void vga_write_at(size_t x, size_t y, const char* str, uint8_t fg, uint8_t bg)
; [ebp + 8]  = x
; [ebp + 12] = y
; [ebp + 16] = str pointer
; [ebp + 20] = fg
; [ebp + 24] = bg
; ------------------------------------------------------------------------
vga_write_at:
    push ebp
    mov ebp, esp
    push esi
    push edi
    push ebx
    push ecx
    push edx

    ; calculate color attribute byte: (bg << 4) | (fg & 0x0F)
    mov eax, [ebp + 24]                    ; bg
    and eax, 0x0F
    shl eax, 4
    mov edx, [ebp + 20]                    ; fg
    and edx, 0x0F
    or eax, edx
    mov dh, al                             ; dh = color attribute

    ; calculate memory offset = (y * 80 + x) * 2
    mov eax, [ebp + 12]                    ; y
    imul eax, 80
    add eax, [ebp + 8]                     ; x
    shl eax, 1                             ; * 2
    add eax, 0xB8000
    mov edi, eax                           ; edi = target VGA buffer address

    mov esi, [ebp + 16]                    ; esi = string pointer
    mov ebx, [ebp + 8]                     ; ebx = cur_x

.loop:
    mov dl, [esi]
    cmp dl, 0
    je .done

    mov [edi], dx                          ; write char (dl) + color (dh)
    add edi, 2
    inc esi
    inc ebx
    jmp .loop

.done:
    ; update cursor to (cur_x, y)
    push dword [ebp + 12]                  ; y
    push ebx                               ; cur_x
    call update_cursor
    add esp, 8

    pop edx
    pop ecx
    pop ebx
    pop edi
    pop esi
    pop ebp
    ret
