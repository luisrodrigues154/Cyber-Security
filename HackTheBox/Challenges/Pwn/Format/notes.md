# Format

## Solution

Initial binary check!

```
    Arch:     amd64-64-little
    RELRO:    Full RELRO
    Stack:    Canary found
    NX:       NX enabled
    PIE:      PIE enabled
```

By the name of the challenge this is a format string vuln (without decompiling and analysis)<br>

To experiment lets send
```
input: WWWW.%x.%x.%x.%x.%x.%x.%x.%x
Output: WWWW.14189a03.0.140b9f4e.6317f610.0.57575757.252e7825.2e78252e
```

So i can access and modify whatever address i source by using the stack position 6 

```
input: WWWW.%6$x
output: WWWW.57575757
```

Now lets analyze the decompilation to see what its possible to do ! <br>

From the decompiled code, nothing much can be seen what leads me to believe that we just need to dump all the stack!!! <br>

After making a script to dump all the stack, the flag was not found this means that we must modify the plt addresses! We need to do this because the read is inside a loop an thus changing the return address leads nowhere. <br>

The idea is:
- leak __libc_start_main address
- change the address of printf to system
- source the "/bin/sh" string in the read (which will be passed to "system")

# I got stuck at this ...
FULL RELRO - cant overwrite the GOT table  
Infinite loop - can rop




