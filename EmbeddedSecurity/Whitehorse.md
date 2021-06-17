# Whitehorse

This level is where we start writing some shellcode to bypass the Hardware security module model 2 (HSM-2) <br>

The first thing we always check is the input and if it complies with the prompt <br>

This time, the prompt says "Between 8-16 characters" and, still, the size passed to **gets** is **0x30** (d:48) <br>

So, we just need to know how much it overflows and, if it impacts the **ret** address, where ! <br>

**Note:** This could've been done just by looking at the instructions (to save cycles) but i just breakpointed at the login's function ret instruction and checked. <br>

So, our overflow is ath the 16th byte ! <br>

We can see that, after receiving input, the function **4446 <conditional_unlock_door>** is called. Well, we cannot control what it does so lets move on to other things <br>

Since what we can control is rather limited with the actual code, why dont we write some code to be executed from memory (if it is allowed)?! Lets write an extra simple shellcode

```assembly
push #0x7f
call #0x4532
```

What this does is extremely simple: invoke the **"syscall"** to unlock the door directly ! From the provided processor manual, we know that this call does not have any arguments makin g our life extremely easy at this level <br>

**Note:** the address use for the call can be retrieved from the code itself <br>

After assembling, our shellcode looks like this: ```30127f00b0123245``` (len = 16/2 = 8) <br>

At this point we need to **pad the buffer** (after the shellcode) with **8 bytes** of junk, so we can have the **shellcode memory address** (at this point, no ASLR of any sort) at the right location to be used by the **ret** instruction <br>

By using the ret breakpoint, we can see that our input is stored at **0x38ce** which means that our final payload will be like so: 
```bash 
$shellcode=30127f00b0123245
$junk_pad=4141414141414141
$jmp_addr=ce38

# payload = 30127f00b01232454141414141414141ce38
```

**Note:** As i always point out, be careful with the endianess (in this case was little endian) <br>

And we unlocked the door :D The **Montevideo** level was also unlocked, see ya next episode <br>