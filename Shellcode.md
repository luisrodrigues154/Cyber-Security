# Shellcode

## Compilation

I always used **NASM** but gcc is awesome because it can cross-compile everything without problems <br>

```bash
gcc -nostdlib -static shellcode.s -o shellcode-elf
```

## Extract shell code opcodes

```bash
objcopy --dump-section .text=shellcode-raw shellcode-elf
``` 

## Basic empty template

```assembly
.global _start
_start:
.intel_syntax noprefix
    xor rax, rax
```

## Strings usage template

Below is a template with the length value harcoded
```assembly
global _start
_start:
.intel_syntax noprefix
        xor rax,rax
        inc rax         ; x86-64 write syscall
        xor rdi,rdi
        lea rsi, [msg]  ; load address of string ; write accepts address to buffer
        xor rdx, rdx    
        jmp get_len     ; jump to len calculator loop
finish:
        mov rdx, rcx         ; set the length for write
        syscall

msg:
.string "its me, waza!\n"

get_len:
    xor rcx,rcx
len_cycle:
    cmp byte[msg+rcx], 0x00
    je finish
    inc rcx
    jmp len_cycle


```
