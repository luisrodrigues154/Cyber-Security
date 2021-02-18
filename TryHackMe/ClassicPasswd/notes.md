# Classic Passwd

## Task1 - Get the flag

I forgot my password, can you give me access to the program? <br>

Starting radare and checking what functions it identified (binary has symbols) we can see
```
0x559074911185    4 260          sym.vuln
```

The function is actually vulnerable to a BOF (using strcpy) but cant really leverage it for anything <br>

Checking the function list again, we can see another function that is not familiar
```
0x559074911289   10 109          sym.gfl
```

This function is called from main
```
0x5590749112ff      e881feffff     sym.vuln ()
0x559074911304      b800000000     eax = 0
0x559074911309      e87bffffff     sym.gfl ()
```

But for this to happen we need to get the username correct so the function does not call exit() before returning (which would lead to not calling the function gfl in main) <br>

we can see it loads the password into memory but a easier way is just to use an application that traces library calls: <b> ltrace</b> <br>

```
cmd: ltrace ./Challenge.Challenge

__isoc99_scanf(0x55e51a34501b, 0x7ffe1e29b3a0, 0, 0Insert your username: test

strcmp("test", "AGB6js5d9dkG7")                                  = 51
```
Well, we got the password ! <br>

Since we got this with ltrace, we could also get this by start debugging and print the strings sent to the strcmp or simply by looking at the assembly
```
0x5590749111dd      48b841474236.  rax = 0x6435736a36424741 ; 'AGB6js5d'
0x5590749111e7      488985c2fdff.  qword [var_23eh] = rax
0x5590749111ee      c785cafdffff.  dword [var_236h] = 0x476b6439 ; '9dkG'
0x5590749111f8      66c785cefdff.  word [var_232h] = 0x37  ; '7' ;
```
Just merge the comments and we get the username too <br>

<b>Answer:</b> THM{65235128496}