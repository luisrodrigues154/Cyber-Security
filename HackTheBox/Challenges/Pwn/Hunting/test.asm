global _start    
section .text

_start:
    jmp call
pop:
    pop ecx                     ; ECX = address of hello
    xor eax, eax                ; EAX = 0
    mov al, 4                   ; EAX = 4 (int 0x80 sys_write)
    xor ebx, ebx
    inc ebx                     ; EBX = 1 (1 = Standard output)
    xor edx, edx
    mov dl, hellolen            ; EDX = length of hello string
    int 0x80

    ; Terminate program
    xor eax, eax
    inc eax                     ; EAX = 1 (int 0x80 sys_exit)
    xor ebx, ebx                ; EBX = return value of 0
    int 0x80
call:
    call pop
    hello: db "Hello World!", 10
hellolen equ $-hello
