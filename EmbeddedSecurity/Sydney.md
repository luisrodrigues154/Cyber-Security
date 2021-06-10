# Sydney

Now the password is not stored in memory <br>

This time, the password is checked in the **check_password** function 

```assembly
448a <check_password>
448a:  bf90 3c3b 0000 cmp	#0x3b3c, 0x0(r15)
4490:  0d20           jnz	$+0x1c
4492:  bf90 4536 0200 cmp	#0x3645, 0x2(r15)
4498:  0920           jnz	$+0x14
449a:  bf90 5735 0400 cmp	#0x3557, 0x4(r15)
44a0:  0520           jne	#0x44ac <check_password+0x22>
44a2:  1e43           mov	#0x1, r14
44a4:  bf90 5e40 0600 cmp	#0x405e, 0x6(r15)
44aa:  0124           jeq	#0x44ae <check_password+0x24>
44ac:  0e43           clr	r14
44ae:  0f4e           mov	r14, r15
44b0:  3041           ret
```

**Note:** for context, r15 stores a memory address which is the location of our input <br>

We can see right away that it seems some sort of indexing ( **0x0(r15)** ) on the input buffer (i dont know arm, and the manual provided is insufficient, but might be its usual syntax tho) <br>

We can see that it verifies two indexes each time against constant values (**#0x3b3c**). If we join all constants together (their hex codes) we have

```bash
# hex constants
3b3c 3645 3557 405e 
# ASCII counterparts
;< 6E 5W @^
```

Now that we know what is the password characters, we need to modify it due to endianess: ```<;E6W5^@```  <br>

And voila, we solved it. What's next? Well it unlocked the **Hanoi** and **Reykjavik** levels :D  