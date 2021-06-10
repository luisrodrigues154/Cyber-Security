# Cusco

This challenge is quite simillar to the previous because we use a programming errors to accomplish the unlock <br>

Here the problem is the same, a buffer overflow. But, in this case, the exploitation is different and extremelly classical :D <br>

**Note:** The prompt still suggests that the passwords must be between 8-16 chars

```assembly
4514:  3e40 3000      mov	#0x30, r14
4518:  0f41           mov	sp, r15
451a:  b012 9645      call	#0x4596 <getsn>
```

Once again we can see that the program differs from the desired actions because it reads **0x30** (48) bytes, when the desired was a maximum of 0x10 (16). <br>

**Note:** This is a 16-bit processor  <br>

But, what can we control? The return address! <br>

```assembly
453a:  3150 1000      add	#0x10, sp
453e:  3041           ret
```

We can see that the stack is advanced (or rewinded?!?!) 0x10 (16) bytes in order to have the return address (for the next instruction) at the top, ready to be popped <br>

But we know that our input is at the top of the stack, and we can store 0x30 (48) bytes instead of 0x10 (16) <br>

With this we can clearly send 0x10 bytes of junk and an address we actually want to return to. Any guess?! Aight, lets return to the ```4446 <unlock_door>``` function <br>

**Note:** Endianess!!! <br>

In this case, you can send upper case **D** and **F** to inject the new return address, or simply send all the input in hex (the UI permits it) <br>

The payload is: ```575757575757575757575757575757574644``` <br>

And voila, door unlocked :D <br>

Levels **Whitehorse** and **Johannesburg** unlocked (oh boy, so many now... grrr)! 