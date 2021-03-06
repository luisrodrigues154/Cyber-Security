# Exatlon

Lets start analyzing the binary with a simple tool that detects if it's packed (Detect-it-easy). Which in fact reports a positive result for UPX packer <br>

Unpacking it, and running it (well its a challenge from a trusted source so i assume its safe) a password is prompted
``` 
███████╗██╗  ██╗ █████╗ ████████╗██╗      ██████╗ ███╗   ██╗       ██╗   ██╗ ██╗
██╔════╝╚██╗██╔╝██╔══██╗╚══██╔══╝██║     ██╔═══██╗████╗  ██║       ██║   ██║███║
█████╗   ╚███╔╝ ███████║   ██║   ██║     ██║   ██║██╔██╗ ██║       ██║   ██║╚██║
██╔══╝   ██╔██╗ ██╔══██║   ██║   ██║     ██║   ██║██║╚██╗██║       ╚██╗ ██╔╝ ██║
███████╗██╔╝ ██╗██║  ██║   ██║   ███████╗╚██████╔╝██║ ╚████║███████╗╚████╔╝  ██║
╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚══════╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝ ╚═══╝   ╚═╝


[+] Enter Exatlon Password  : password
[-] ;(
```

Well, it does this forever so lets see how it operates (I'll use IDA PRO 7.0) <br>

Checking its strings we can see several interesting things
```
.rodata:000000000054B4D0	0000001F	C	[+] Enter Exatlon Password  : 
.rodata:000000000054B4F0	000000AB	C	1152 1344 1056 1968 1728 816 1648 784 1584 816 1728 1520 1840 1664 784 1632 1856 1520 1728 816 1632 1856 1520 784 1760 1840 1824 816 1584 1856 784 1776 1760 528 528 2000 
.rodata:000000000054B59B	00000017	C	[+] Looks Good ^_^ \n\n\n
.rodata:000000000054B5B4	00000008	C	[-] ;(\n
```

A 36 character string is present which i assume that is the password with some sort of obfuscation (encription, encoding, etc) <br>

Luckly we have our **main** entry function available (the binary is compiled with symbols which is really helpfull) and looking at the code we can see that it is c++ <br>

The first thing i did was trying to figure out how the program jumps to **"Looks Good"** which indicates that the password is correct, and also identify the **";("** as well to visualize how the program actually flows <br>

```
0000000000404D4E                 lea     rsi, aLooksGood ; "[+] Looks Good ^_^ \n\n\n"
0000000000404DA7                 lea     rsi, asc_54B5B4 ; "[-] ;(\n"
```

We can see that before it arrives at the failed password portion, it checks if the input is **q** so it closes the program (well dont need to send a SIGINT again xD) <br>

Lets analyze the portion of code that actually obfuscate's our input and check against the stored password (i renamed it to be easier to reference it) <br>

```
0404CF0                 lea     rsi, aEnterExatlonPa ; "[+] Enter Exatlon Password  : "
0404CF7                 lea     rdi, _ZSt4cout ; std::ostream *
0404CFE ;   try {
0404CFE                 call    write_output ;
0404D03                 lea     rax, [rbp+var_50]
0404D07                 mov     rsi, rax
0404D0A                 lea     rdi, _ZSt3cin ; std::cin
0404D11                 call    read_input; 
0404D16                 lea     rax, [rbp+var_30]
0404D1A                 lea     rdx, [rbp+var_50]
0404D1E                 mov     rsi, rdx
0404D21                 mov     rdi, rax
0404D24                 call    exatlon_input
0404D29                 lea     rax, [rbp+var_30]
0404D2D                 lea     rsi, a11521344105619 ; "1152 1344 1056 1968 1728 816 1648 784 1"...
0404D34                 mov     rdi, rax
0404D37                 call    str_cmp;
0404D3C                 mov     ebx, eax
0404D3E                 lea     rax, [rbp+var_30]
0404D42                 mov     rdi, rax
0404D45                 call    check_empty;     <- NOT SURE
0404D4A                 test    bl, bl
0404D4C                 jz      short loc_
```

From the code above we can see that it:
- [0404D03 - 0404D11] Loads the address in **rbp-var_50** to **rax** and calls **cin**
  - The address at rbp-var_50 was allocated before, but that is not really important right now
- [0404D16 - 0404D24] 
  - Loads another two address
    - **rbp-var_30** -> rax
    - **rbp-var_50** -> rdx
  - Move them into arguments registers and calls exatlon. This indicates that the input after being processed will be stored at **rbp-var_30**. This is confirmed right after
-  [0404D29 - 0404D37] Loads the result of the obfuscation and the password stored in the .data section and compares them
-  [0404D3C] Moves the result of the comparison int **ebx** 
-  [0404D3E - 0404D45] Not sure, but seems to check if the generated string is NULL/empty
-  [0404D4A - 0404D4C] Test the LO bit of **ebx** (with bl)
   -  If it is 0 -> wrong password
   -  Otherwise -> correct password

Now that we see how it verifies and operates, we need to check how the exatlon in fact obfuscates so we can reverse and insert the correct password <br>

If we follow the exatlon address (from main), we can see (in IDA) multiple flows that are not interconnect which indicates that this is class and have several methods. But, from the call in main, we can see some things<br>
- The initial box that is visible when double clicking the exatlon call in main is just stack frame initialization and allocations, and we can forget it for now (if we need after, we can return here)
- Then we can see that a loop exists (it is always reached from the from the initial "landing place") by the below code
```
lea     rdx, [rbp+var_78]                                   <- len (check next piece to see why this and the next line are what i describe as )
lea     rax, [rbp+var_70]                                   <- Iterator 
mov     rsi, rdx
mov     rdi, rax
call    check_loop_condition ; __gnu_cxx::operator!= (...)  <-  we can see from IDA decompilation that it uses the operator !=
test    al, al                                              <-  Test the return from loop condition checking
jz      loc_404C1A                                          <-  If 0 (a!=b == 0 if a==b ), exit the loop
``` 

So what we need to check now is what happens in each iteration. It is verbose so i will just paste the essential parts

```

lea     rax, [rbp+var_70]               <- Loads the value of i (i'm saying i because is the common iterator)
mov     rdi, rax                        <- Moves to argument
call    get_value_for_idx;              <- Gets a byte for the index
movzx   eax, byte ptr [rax]             <- Loads the returned byte and zero extends it (var = str[rax])
mov     [rbp+var_19], al                <- Move the LO bits of the byte to a variable
movsx   eax, [rbp+var_19]               <- Moves the byte to eax and sign extendes it (code is not optimized, this is'nt needed)
shl     eax, 4                          <- Shift's left 4 over the byte 
mov     edx, eax
lea     rax, [rbp+var_40]
mov     esi, edx        ; int           <- Converts to INT
mov     rdi, rax        ; this          <- loads the class instance to argument
;   try {
call    _ZNSt7__cxx119to_stringEi ; std::__cxx11::to_string(int)    <- Tries to cast to string 
```

We can already understand what the program does
1. Uses a cycle to iterate over the input
2. Shifts left each byte by 4 bits
3. Returns and compares with the password in .data

So, in order to have the solution we need
1. Convert the value to decimal
2. Shift right by 4
3. Convert to ascii

I figured the decimal part playing with an [online](https://bit-calculator.com/bit-shift-calculator) shifter <br>

FLAG: HTB{l3g1c3l_sh1ft_l3ft_1nsr3ct1on!!}

Note: The python script is in this note folder
