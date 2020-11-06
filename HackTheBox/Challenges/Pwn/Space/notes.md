# Space 

IP:PORT = 165.232.96.76:30062

## Solution

Lets start by checking the binary security features 

```
cmd: checksec --file=space

No RELRO 
No canary found   
NX disabled   
No PIE          
No RPATH   
No RUNPATH   
70) Symbols       
```

As seen, we can exploit it pretty much in any way <br>

Got nothing interesting from <b> strings </b> command <br>

Lets load it on Ghidra <br>

The vulnerable function receives a 31 bytes sized buffer and copies it to a 10 bytes buffer <br>

To copy it uses strcpy which copies until a null terminator ! <br>

With 30 bytes available, which might be enough for shell code, is safer to do a ret2libc <br>

Unfortunately, ret2libc will not work since we have a dynamically linked binary and we dont know the libc version it is using (it can be leaked but is not that reliable)... we need to execute the shell code in the stack! <br>

I was failing to exploit this but i saw a hint that made me remind a very interesting thing:
- What you dont read stay there.... <br>

When the program reads 31 bytes from stdin, if more is sourced, it can still be read because the data that was not consumed still in the pipe (stdin) <br>

With this, if the payload is a two stage payload meaning:
- First stage: trigger another read
- Second stage: read unconsumed data in stdin (which contains shell code)

The read will refetch the buffer with the remaining data without affecting anything !!! <br>

```
THE SCRIPT IS INCLUDED IN THIS REPO FOLDER! (with explanations)
```


FLAG: HTB{sh3llc0de_1n_7h3_5p4c3}


