# Shellcode

## Compilation

I always used **NASM** but gcc is awesome because it can cross-compile everything without problems <br>

```bash
gcc -nostdlib -static shellcode.s -o shellcode-elf
```

## Extract shell code opcodes

To file 

```bash
objcopy --dump-section .text=shellcode-raw shellcode-elf
``` 


## Compilation + extraction to file (one line)

```bash
gcc -nostdlib -static shellcode.s -o shellcode-elf && objcopy --dump-section .text=shellcode-raw shellcode-elf
```

As escaped string
```bash
# Get all opcodes (raw)
objdump -d ./your_program|grep '[0-9a-f]:'|grep -v 'file'|cut -f2 -d:|cut -f1-6 -d' '|tr -s ' '|tr '\t' ' '|sed 's/ $//g'|paste -d '' -s |sed 's/^/"/'|sed 's/$/"/g'

# Get all opcodes (shellcode format)
objdump -d ./your_program|grep '[0-9a-f]:'|grep -v 'file'|cut -f2 -d:|cut -f1-6 -d' '|tr -s ' '|tr '\t' ' '|sed 's/ $//g'|sed 's/ /\\x/g'|paste -d '' -s |sed 's/^/"/'|sed 's/$/"/g'
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
.global _start
_start:
.intel_syntax noprefix 

do_write:               ; starts write syscall
    xor rax,rax             
    inc rax
    lea rsi, [mystr]
    jmp getstrlen       ; jmps to string length calc
continue:
    xor rdx, rdx 
    add rdx, rcx
    syscall             ; calls write

finish:                 ; start exit syscall
    mov rax, 60
    syscall             ; calls exit

getstrlen:
    xor rcx, rcx
    xor rbx, rbx
iteration:
    mov dl, BYTE [rsi+rcx]  ; copies byte index by rcx
    cmp dl, 0               ; if null, write str
    je continue
    inc rcx                 ; increment counter
    jmp iteration           ; continue looping

mystr:
.string "sure\n"
```

## C-code shell executor

Usefull when need other code prior (avoiding writing all in assembly)

```c
page = mmap(0x1337000, 0x1000, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANON, 0,0);
read(0, page, 0x1000);
((void (*)())page)();
```

