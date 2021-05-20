# DebugMe

A develper is experiementing with different ways to protect their software. <br> 
They have sent in a windows binary that is suposed to super secureand really hard to debug. Debug and see if you can find the flag.

## Solution

First thing i did was run the program, cmd opens and closes. That's all ! <br>

Went straight into IDA to check what we can learn from it... well, a bunch of functions but only some exports!
- TlsCallback_0     -> old anti-debugging trick to use the TLS callback which execs before entrypoint
- TlsCallback_1
- _mainCRTStartup   -> main prolog?

Then i just loaded the **.exe** into **PEStudio** in order to identify its entry point (i know IDA has it, but not that used to it): 
- Entry point address: 4010F9
- IDA Function: _mainCRTStartup
  
```assembly
public _mainCRTStartup
    jmp     _mainCRTStartup_0   # jmp 0x408995
_mainCRTStartup endp
```
IDA cant decompile it because it jumps to **0x408995**, which does not exist in the executable (last function at **0x408904**). <br> 

This is a strong indication that the code might modify itself afterwards. <br>

Although we cannot generate a pseudo-code for that function, we can see its instructions (assembly) in the decompiler view <br> 

I sat a breakpoint at that call, to see if it is actually hit (and to check its execution) or if the TlsCallback would interfere with it! <br>

It got hit <br>

In the whole function code there's a block that needs to be avoided because its functionality is to exit and the code within this function always branch true to it (so i assume that those true statements are forms of identifying a debugger)

```assembly
loc_408992:
    mov     esp, ebp
    pop     esp
    retn
_mainCRTStartup_0 endp ; sp-analysis failed
```

To avoid that code section, we need to bypass 3 statements

```bash
# ----------------------------
# Statement 1
#_____________________________
cmp     al, 0
jnz     short loc_408992
# ----------------------------
# Statement 2
#_____________________________
cmp     al, 0
jnz     short loc_408992
# ----------------------------
# Statement 3
#_____________________________
cmp     eax, 3E8h
jg      short loc_408992
#_____________________________
```

Bypass for 1 and 2 (manual): set AL (eax less significant bytes) to 0 <br>
Bypass for 3 (manual): set eax to 0 <br>

Within this function, there's a block of code that confirms the selfmodifying code.  

```assembly
# -- bunch of nops --  
nop
mov     eax, offset _main           # Loads buffer address of encoded/encrypted code

loc_408973:
    xor     byte ptr [eax], 5Ch     # Decodes/decrypts with xor 5C
    inc     eax                     # advances buffer indexer
    cmp     eax, offset loc_401791  # checks if everything decoded/decrypted
    jle     short loc_408973        # next iteration
```

After this i accidentially clicked to continue instead of step and it got catched by some anti-debugging mechanis.

```sql
-- If the debugger is catched!
"Looks like your doing something naughty. Stop it!!!"
```


Repeating the whole process, we arrive at: **call ___tmainCRTStartup** <br>

If we analyze more carefully, we can skip most of the code since great part of it is relate to mingw stuff, callbacks to exceptions and stuff that we dont actually need <br>

After some trial and error i found out that we can jump directly (by setting the EIP) to the main call "preparation" which starts at **0x0040132A2** 

```assembly
0x004013A2 mov eax, ds:__bss_start__    # SET EIP HERE
```

Then we can step into a couple of instruction. <br>

I also jumped ```0x04013B7 call  ___main``` which, also, triggers an error (dont actually know if it is an anti-debugging guard or just because i step over stuff it actually needs). <br>

But, by its name, seems that it is intended to mislead and execute it xD <br>

After that we enter into the real main function at: ```0x04013E5 call    _main```  <br>

Here we can see the decoded/decrypted code being executed and we need to be carefull because the code is very simillar to the initial one that we needed to bypass ! (the eax related one) <br>

These bypasses (and the prior we could've done, very likely, in the same manner), we can simply put EIP after the **cmp's** to short jump 5 instructions further

```assembly
.text:00401630 mov     dl, al
.text:00401632 cmp     al, 0        # EIP HERE
.text:00401634 jnz     short _d
.text:00401636 jmp     $+5          # SET EIP HERE

--- snip ---
.text:0040164A cmp     al, 0        # EIP HERE
.text:0040164C jnz     short _d
.text:0040164E rdtsc                # SET EIP HERE

--- snip ---
.text:00401674 cmp     eax, 3E8h    # EIP HERE
.text:00401679 jg      short _d
.text:0040167B jmp     _n           # SET EIP HERE
```

From here just execute until the first ret 
```assembly
text:00401791 retn
```

**Note:** the program misbehave between two runs but you might not even reach the retn and it will produce an error and hang (it didn't quit in IDA) <br>

Then the following message appeared on the terminal 

```sql
"I heard you like bugs so I put bugs in your debugger so you can have bugs while you debug!!!
Seriously though try and find the flag, you will find it in your debugger!!!"
```

I just went to check IDA strings, and it was filled with stuff so i just searched for the latter part of the above statement and it was there. <br>

So i decided to try to find if the flag was really there :p  <br>

What i did was (in this order):
1. Set displayed strings size  >10 (just in case it is small, but is usually 20+)
2. Remove any .dll related strings using IDA built-in filter (Addresses **containing** KernelBase, ntdll, msvcrt, wow64cpu **excluded**)
3. Visually analyzed strings from **.data** and  **.text** and was nothing there (Remove them with filter)
   - Found The other error message from before (together with the last seen) ... **check Note1**
4. Found that there were still a few sections (debug011, debug023, debugme.exe, Stack)
   - That stack one seems interesting, haven't seen that before ... **check Note 2**

```sql
-- NOTE 1
-- We can see the outputed strings in the .data section but flag not there
.data:00409000	00000035	C	"Looks like your doing something naughty. Stop it!!!\n"
.data:00409035	0000005E	C	"I heard you like bugs so I put bugs in your debugger so you can have bugs while you debug!!!\n"
.data:00409093	0000004E	C	"Seriously though try and find the flag, you will find it in your debugger!!!\n"
```

```sql
-- NOTE 2
-- between some strings there is the flag!!!
Stack[00001304]:0061FF28	00000025	C	"Tr0lling_Ant1_D3buGGeR_trickz_R_fun!p"
```

Well, just for the troll, i tried it but it needed the trailling **p** to be removed <br> 

### FLAG: HTB{Tr0lling_Ant1_D3buGGeR_trickz_R_fun!}