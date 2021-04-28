# Bad grades

"You are not interested in studying for school anymore, you only play CTFs and challenges!  <br>
Your grades fell off a cliff! I will take your laptop away if you continue like this". You need to do something to raise them before your parents ground you forever..

# Solution

The challenge files are comprised of 2 elements:
- Bad grades binary
- Libc binary (a shared lib)

Lets start by analyzing how the **bad_grades** binary was compiled 
```bash
cmd: file bad_grades

bad_grades: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, for GNU/Linux 3.2.0, BuildID[sha1]=b60153cf4a14cf069c511baaae94948e073839fe, stripped

cmd: checksec --file=bad_grades
RELRO         - Full RELRO     
STACK CANARY  - Canary found       
NX            - NX enabled
PIE           - No pie          
RPATH         - No RPATH      
RUNPATH       - No RUNPATH             
Symbols       - No Symbols      
FORTIFY       - No  
Fortified     - 0  
Fortifiable   - 1      
FILE          - bad_grades      
``` 

First bad news: the binary is stripped, has canaries and cannot execute shell code in the stack <br>

The strategy for exploitation, since we have a libc, is certainly the following:
1. Find a format string vulnerability
2. Leak the canary
3. Leak libc base address
4. Return to libc

Before jumping into the disassembly, lets see how the program behaves <br>

```sql
Your grades this semester were really good BAD!

1. View current grades.
2. Add new.
> 1

Your grades were: 
2
4
1
3
0

You need to try HARDER!
-- program exited here

------------------------------------------------------
Your grades this semester were really good BAD!

1. View current grades.
2. Add new.
> 2

Number of grades: 2
Grade [1]: 12\n
Grade [2]: Your new average is: 6.00

``` 

We can see that the behaviour is odd and not correct, tere are more forms of breaking it
```sql 
Number of grades: AAAAAAAAAAAAAAAAAAAAA
Your new average is: -nan

``` 

Very large numbers dont break it as well as negative numbers (at least, it seems) <br>

Lets jump into the code, shall we?!