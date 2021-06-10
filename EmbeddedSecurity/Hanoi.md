# Hanoi

From the introduction to the challenge, we can see that the lock is attached to the HSM-1 (hardware secure module) <br>

From the manual we can see that it is possible to reset the password the HSM stores by detaching/attaching the module. But we have no means to do that <br>

The first function that requires our attention is the **test_password_valid**, for obvious reasons
```assembly
4454 <test_password_valid>
4454:  0412           push	r4
4456:  0441           mov	sp, r4
4458:  2453           incd	r4
445a:  2183           decd	sp
445c:  c443 fcff      mov.b	#0x0, -0x4(r4)
4460:  3e40 fcff      mov	#0xfffc, r14
4464:  0e54           add	r4, r14
4466:  0e12           push	r14
4468:  0f12           push	r15
446a:  3012 7d00      push	#0x7d
446e:  b012 7a45      call	#0x457a <INT>
4472:  5f44 fcff      mov.b	-0x4(r4), r15
4476:  8f11           sxt	r15
4478:  3152           add	#0x8, sp
447a:  3441           pop	r4
447c:  3041           ret
```

Although its name seems promising, this function fails to deliver something usefull (as far as i can tell tho) <br>

We can see the interrupt in action, to jump to the HSM code (the manual references **0x7d** as the signal to accomplish that, and we can see it at **446a**) <br>

Then, at the **login** function, there's a logic error or simply a backdoor 
```assembly
4544:  b012 5444      call	#0x4454 <test_password_valid>
4548:  0f93           tst	r15
454a:  0324           jz	$+0x8
454c:  f240 f300 1024 mov.b	#0xf3, &0x2410
4552:  3f40 d344      mov	#0x44d3 "Testing if password is valid.", r15
4556:  b012 de45      call	#0x45de <puts>
455a:  f290 6200 1024 cmp.b	#0x62, &0x2410
4560:  0720           jne	#0x4570 <login+0x50>
4562:  3f40 f144      mov	#0x44f1 "Access granted.", r15
```

Why there's an error/backdoor?
- **4548**: the value returned by te HSM-1 is tested
- **454a**: we jump to **4552** because it was 0 (HSM-1 sets to 1 if correct)
- **455a**: After the string (testing ...) is printed, it is tested if the byte at **0x2410** is **0x62** (b). **IF IT IS**, print access granted and unlock

From the live memory dump window, we can see that our input starts at **0x2400** and it checks the **0x2410** which is 17 bytes into the buffer <br>

Now, althought the password prompt refers that it requires a password with a length between 8-16, if we send 17 (where the 17th character is **b**), we can satisfy the **cmp.b** and unlock the door. <br>

But is this possible? We can check it by analyzing the call to the **getsn** function 

```assembly
4520 <login>
4520:  c243 1024      mov.b	#0x0, &0x2410
4524:  3f40 7e44      mov	#0x447e "Enter the password to continue.", r15
4528:  b012 de45      call	#0x45de <puts>
452c:  3f40 9e44      mov	#0x449e "Remember: passwords are between 8 and 16 characters.", r15
4530:  b012 de45      call	#0x45de <puts>
4534:  3e40 1c00      mov	#0x1c, r14
4538:  3f40 0024      mov	#0x2400, r15
453c:  b012 ce45      call	#0x45ce <getsn>
```

We can see the gets' arguments in two instructions prior to the **call**. This is strange because the documentation states that the arguments are passed via **stack** and here it uses **registers** to accomplish that <br>

From the documentation provided, we can see that the **gets** prototype is
```c
void gets(char* buf, unsigned int length);
```

Which means that:
- **4534**:  #0x1c -> size (28) 
- **4538**:  #0x2400 -> input mem address (as seen in the live memory)

So, we can clearly input more (28) than the prompt states (16) and consequently bypass the call to HSM-1 and thus its password verification. <br>


And with the special input **AAAAAAAAAAAAAAAAb**, we can unlock it :D <br>

Levels **Cusco** and **Addis Ababa** unlocked !! <br>