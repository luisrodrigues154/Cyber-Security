.globl _start
.intel_syntax noprefix

_start:
    xor eax, eax
	xor ebx, ebx
    mov al, 1
	mov bl, 99
    int 0x80
