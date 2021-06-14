# Reykjavik

In this one, we are prompted with the traditional message with some information regarding the challenge. <br> 

This time we have an interesting thing: This release contains military-grade encryption so users can be confident that the passwords they enter can not be read from memory. <br>

From prior levels, lets check right away if we still have an overflow ! :) <br>

And.... there seems to be no **gets** function, but it prompts for a password! Self modifying code via encryption?! hummm <br>

We can see that it will happen in fact

```assembly
4438 <main>
4438:  3e40 2045      mov	#0x4520, r14
443c:  0f4e           mov	r14, r15
443e:  3e40 f800      mov	#0xf8, r14
4442:  3f40 0024      mov	#0x2400, r15
4446:  b012 8644      call	#0x4486 <enc>
444a:  b012 0024      call	#0x2400
444e:  0f43           clr	r15
```

Why? because we can see values **0xf8** and **0x2400** being passed to the **enc**(ryption) function and, then, the later is called. <br>

Noting that **0x2400** is unmapped from the start, so it is being allocated and then the encrypted code stored there?! <br>

The encryption function looks like this

```assembly
4486 <enc>
4486:  0b12           push	r11
4488:  0a12           push	r10
448a:  0912           push	r9
448c:  0812           push	r8
448e:  0d43           clr	r13
4490:  cd4d 7c24      mov.b	r13, 0x247c(r13)
4494:  1d53           inc	r13
4496:  3d90 0001      cmp	#0x100, r13
449a:  fa23           jne	#0x4490 <enc+0xa>
449c:  3c40 7c24      mov	#0x247c, r12
44a0:  0d43           clr	r13
44a2:  0b4d           mov	r13, r11
44a4:  684c           mov.b	@r12, r8
44a6:  4a48           mov.b	r8, r10
44a8:  0d5a           add	r10, r13
44aa:  0a4b           mov	r11, r10
44ac:  3af0 0f00      and	#0xf, r10
44b0:  5a4a 7244      mov.b	0x4472(r10), r10
44b4:  8a11           sxt	r10
44b6:  0d5a           add	r10, r13
44b8:  3df0 ff00      and	#0xff, r13
44bc:  0a4d           mov	r13, r10
44be:  3a50 7c24      add	#0x247c, r10
44c2:  694a           mov.b	@r10, r9
44c4:  ca48 0000      mov.b	r8, 0x0(r10)
44c8:  cc49 0000      mov.b	r9, 0x0(r12)
44cc:  1b53           inc	r11
44ce:  1c53           inc	r12
44d0:  3b90 0001      cmp	#0x100, r11
44d4:  e723           jne	#0x44a4 <enc+0x1e>
44d6:  0b43           clr	r11
44d8:  0c4b           mov	r11, r12
44da:  183c           jmp	#0x450c <enc+0x86>
44dc:  1c53           inc	r12
44de:  3cf0 ff00      and	#0xff, r12
44e2:  0a4c           mov	r12, r10
44e4:  3a50 7c24      add	#0x247c, r10
44e8:  684a           mov.b	@r10, r8
44ea:  4b58           add.b	r8, r11
44ec:  4b4b           mov.b	r11, r11
44ee:  0d4b           mov	r11, r13
44f0:  3d50 7c24      add	#0x247c, r13
44f4:  694d           mov.b	@r13, r9
44f6:  cd48 0000      mov.b	r8, 0x0(r13)
44fa:  ca49 0000      mov.b	r9, 0x0(r10)
44fe:  695d           add.b	@r13, r9
4500:  4d49           mov.b	r9, r13
4502:  dfed 7c24 0000 xor.b	0x247c(r13), 0x0(r15)
4508:  1f53           inc	r15
450a:  3e53           add	#-0x1, r14
450c:  0e93           tst	r14
450e:  e623           jnz	#0x44dc <enc+0x56>
4510:  3841           pop	r8
4512:  3941           pop	r9
4514:  3a41           pop	r10
4516:  3b41           pop	r11
4518:  3041           ret
```

We can see that it is **xor'ing** all the indexes at **0x2400** with values starting at **0x247c**
```assembly
# snip
xor.b	0x247c(r13), 0x0(r15)
# snip
```

Although the function name ins **enc**(ryption), this is actually decrypting !! <br>

Now, we can just see what is being decrypted by inspecting the same memory addresses right after we send the input (the debugger automatically stops after an **INT**). <br> 

It is also important to save the instruction right after we send the input (and press **o** to step out of the **INT** function), through the top-right box **"Current Instruction"** <br>

Current instruction (after input and step out): 
```assembly
add #0x6, sp
```

Remember that the important part is between **0x2400** (it was encrypted before) and **0x247c** (the key starting address)

```r
2400:   0b12 0412 0441 2452 3150 e0ff 3b40 2045   .....A$R1P..;@ E
2410:   073c 1b53 8f11 0f12 0312 b012 6424 2152   .<.S........d$!R
2420:   6f4b 4f93 f623 3012 0a00 0312 b012 6424   oKO..#0.......d$
2430:   2152 3012 1f00 3f40 dcff 0f54 0f12 2312   !R0...?@...T..#.
2440:   b012 6424 3150 0600 b490 eed4 dcff 0520   ..d$1P......... 
2450:   3012 7f00 b012 6424 2153 3150 2000 3441   0....d$!S1P .4A
2460:   3b41 3041 1e41 0200 0212 0f4e 8f10 024f   ;A0A.A.....N...O
2470:   32d0 0080 b012 1000 3241 3041   c   d
``` 

Using the given disassembler by the platform, we can decompile this to assembly instructions (can paste that stuff right away, it does not require opcodes only) !

```assembly
# snip 
0f12           push	r15
2312           push	#0x2
b012 6424      call	#0x2464
3150 0600      add	#0x6, sp            <- WE ARE HERE
b490 eed4 dcff cmp	#0xd4ee, -0x24(r4)
0520           jnz	$+0xc
|    3012 7f00      push	#0x7f
|    b012 6424      call	#0x2464
|    2153           incd	sp
V
3150 2000      add	#0x20, sp
3441           pop	r4
3b41           pop	r11
3041           ret
1e41 0200      mov	0x2(sp), r14
0212           push	sr
0f4e           mov	r14, r15
8f10           swpb	r15
024f           mov	r15, sr
32d0 0080      bis	#0x8000, sr
b012 1000      call	#0x10
```

We can see right away that, after we step out of the gets function, the code compares whatever is at **r4-0x24** with **0xd4ee** (constant value). **IF** it is **EQUAL**, do the **INT**errupt seen before to unlock (**0x7f**) <br>

Now, can we control **r4-0x24**?? 

```bash
cmp	#0xd4ee, -0x24(r4)
r4      -> 43fe
r4-0x24 -> 43da -> sp -> 4141
```

We can clearly see that it is comparing with our input directly, so, we just need to provide **eed4** (endianess) to pass the verification and unlock the door :D !! (use the hex encoded input option tho) <br>

And we unlocked the door, and the **Johannesburg** level !!