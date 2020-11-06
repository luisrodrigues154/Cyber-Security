# You Know 0xDiablos

IP:PORT = 134.122.99.74:30609


## Solution

Loading the binary in ghidra we can clearily see that this is vulnerable due to the gets function usage <br>

Probably is the trivial buffer overflow to modify the return address ! <br>

What we need to call is a function called: <b> flag(int, int)</b>

The function opens the "flag.txt" file and reads 0x40 bytes from it 

```
local_10 = fopen("flag.txt","r");
(...)
fgets(local_50,0x40,local_10);
```

Then do the following comparison with the arguments passed

```
if ((param_1 == -0x21524111) && (param_2 == -0x3f212ff3)) {
    printf(local_50);
}
```

The easier way would be to just ret2libc and call /bin/sh but i think this is not the intended form of solving this challenge! <br>

I dont know why, but ghidra gave me those values but from the GDB we can visualize the actual hexadecimal values !!!

```
param_1 : 0xdeadbeef
param_2 : 0xc0ded00d
```

So we just need to craft a script to solve this on the remote !! (on this folder: <b> exp.py </b>) <br>

FLAG: HTB{0ur_Buff3r_1s_not_healthy}
