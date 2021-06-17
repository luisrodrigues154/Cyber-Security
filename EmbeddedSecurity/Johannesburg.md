# Johannesburg

This challenge was left behind because i overlooked its points (i solve the ones with lower rewards first), but lets go ! <br>

From the prompt (used to provide input) we can see the previously seen message
```sql
Enter the password to continue.
Remember: passwords are between 8 and 16 characters.
```

But, also as seen previously, this might not be well sanitized/checked. First thing to check? can we write more than 16?? <br>

And the answer is: Yes we can 

```assembly
4546:  3e40 3f00      mov	#0x3f, r14      ; <----- the size 0x3f (d:63)
454a:  3f40 0024      mov	#0x2400, r15    ; <----- input saving location 
454e:  b012 e845      call	#0x45e8 <getsn>
```

So we can overwrite, but, how is the password verified? This is where this challenge distincts from the previous ones ! <br>

**Note:** it also uses strcpy, but in this case isn't much of a problem since it is already overflown via the input

```assembly
455e:  b012 5244      call	#0x4452 <test_password_valid>
4562:  0f93           tst	r15
4564:  0524           jz	#0x4570 <login+0x44>
4566:  b012 4644      call	#0x4446 <unlock_door>
456a:  3f40 d144      mov	#0x44d1 "Access granted.", r15
456e:  023c           jmp	#0x4574 <login+0x48>
4570:  3f40 e144      mov	#0x44e1 "That password is not correct.", r15
4574:  b012 f845      call	#0x45f8 <puts>
4578:  f190 ca00 1100 cmp.b	#0xca, 0x11(sp)
457e:  0624           jeq	#0x458c <login+0x60>
4580:  3f40 ff44      mov	#0x44ff "Invalid Password Length: password too long.", r15
4584:  b012 f845      call	#0x45f8 <puts>
4588:  3040 3c44      br	#0x443c <__stop_progExec__>
458c:  3150 1200      add	#0x12, sp
4590:  3041           ret
```

We see that it makes a special check that most certainly our input can manipulate! <br>

```assembly
4578:  f190 ca00 1100 cmp.b	#0xca, 0x11(sp)
```

The program verifies if the **17th** index of our payload is equal to **0xca**. IF IT IS, it jumps to a normal return, otherwise it forces the **__stop_progExec__** function ! <br>

What this basically is?! A static canary! Right after it, we find the return address<br>

**Note:** Instead of jumping directly to the **unlock_door** function, i jumped to the instruction that calls it within the **login (4566)** function. Also, i sent the bytes as characters

The winning input is: ```AAAAAAAAAAAAAAAAAÊfE```

**Note:** Remember endianess (little endian in this challenge's case)!!

And the door is unlocked and as well as the **Santa Cruz** level :D 