# Reverse Engineering

## Task 2 - crackme1

This first crackme file will give you an introduction to if statements and basic function calling in assembly. <br>

The password can be extracted right away without deeper analysis with
```
cmd: strings crackme1.bin

(...)
hax0r
(...)
```

But i'll analyze the binary to check the password within the assembly <br>

Well, this was really supposed to be found with just strings
```
0x561036a0078d      8b053d010000   eax = dword [str.hax0r] ; [0x561036a008d0:4]=0x30786168 ; "hax0r"
0x561036a00793      8945ec         dword [var_14h] = eax
(...)
0x561036a007b9      488d55ec       rdx = var_14h <- "hax0r" str
0x561036a007bd      488d45f2       rax = var_eh  <- input
0x561036a007c1      4889d6         rsi = rdx
0x561036a007c4      4889c7         rdi = rax
0x561036a007c7      e864feffff     sym.imp.strcmp ()
```
<b>Answer:</b> hax0r

## Task 3 - crackme2

This time the password is not present in the data section, so strings wont find it! <br>

It is not present because the password is supposed to be an integer (and so, strings wont get it, duh) <br>

```
0x556a01c0073d      488d45f4       rax = var_ch             <- Where the password will be stored
0x556a01c00741      4889c6         rsi = rax
0x556a01c00744      488d3ded0000.  rdi = rip + 0xed        ; "%d"  <- reads an int
                                                           ; 0x556a01c00838
0x556a01c0074b      b800000000     eax = 0
0x556a01c00750      e89bfeffff     sym.imp.__isoc99_scanf () ; int scanf(const char *format)
```

Now, we can check where it redirects in case it fails or succeeds
```
0x556a01c0075d      750e           if (var) goto 0x556a01c0076d
0x556a01c0075f      488d3dd50000.  rdi = rip + 0xd5        ; str.password_is_valid
```

We can see if the var is true (it is different than 0), it jumps somewhere other than what we want (password is valid) <br>

var is calculated with
```
0x556a01c00755      8b45f4         eax = dword [var_ch]
0x556a01c00758      3d7c130000     var = eax - 0x137c
```
So, the password must be 0x137c (4988) to result in 0 and be accepted <br>

<b>Answer:</b> 4988

## Task 4 - crackme3

By quickly analyzing the binary, we can see some chars being loaded to rbp-0x23
```
0x55ce67800731      66c745dd617a   word [var_23h] = 0x7a61 ; 'az'
0x55ce67800737      c645df74       byte [var_21h] = 0x74   ; 't' ; 116
```

This gives the solution right away (azt) but by analyzing the loop, we can see that it implements a strcmp between that memory address and the sourced string because we have some indexing
```
0x55ce67800747      488d45e0       rax = var_20h            <- where input string is stored
0x55ce6780074b      4889c6         rsi = rax
0x55ce6780074e      488d3d130100.  rdi = rip + 0x113       ; "%s"
                                                           ; 0x55ce67800868
0x55ce67800755      b800000000     eax = 0
0x55ce6780075a      e891feffff     sym.imp.__isoc99_scanf () ; int scanf(const char *format)

(...)

0x55ce67800768      8b45d8         eax = dword [var_28h]            <- loads loop indexer into eax
0x55ce6780076b      4898           cdqe                             <- sign-extends eax and stores in RAX
0x55ce6780076d      0fb65405e0     edx = byte [rbp + rax - 0x20]    <- indexes user input with RAX, and gets a byte (a char is assured to be 1 byte in C)
0x55ce67800772      8b45d8         eax = dword [var_28h]
0x55ce67800775      4898           cdqe
0x55ce67800777      0fb64405dd     eax = byte [rbp + rax - 0x23]    <- indexes password with the same RAX, and gets a byte (a char is assured to be 1 byte in C)
0x55ce6780077c      38c2           var = dl - al                    <- check if the bytes are equal by comparing the LO.bits of edx and eax
0x55ce6780077e      7413           if (!var) goto 0x55ce67800793    <- if false, jumps to the incrementer
(...)
0x55ce67800793      8345d801       dword [var_28h] += 1             <- the incrementer
```
So, stopping the debugger and print rbp-0x23 will give us: aztgU <br>

The loop structure is: i = -2; i<=0 ; i++ <br>

This means that only the first 3 characters are important, giving <b>azt</b> which is the correct answer <br>

<b> Answer: </b> azt