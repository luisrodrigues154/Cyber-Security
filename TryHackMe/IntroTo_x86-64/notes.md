# Intro to x86-64

This only stores the answers, not the explanations and stuff

## Task 4 - If statements

The following questions involve analysing the if2 binary. <br>

1. What is the value of var_8h before the popq and ret instructions?
```
> db pop rbp
> px @ rbp-0x8
60 
```
Answer: 96 (decimal, lol)<br>
2. what is the value of var_ch before the popq and ret instructions?
```
(already at the breakpoit)
> px @ rbp-0xc
00
```
Answer: 0 <br>
3. What is the value of var_4h before the popq and ret instructions?
```
(already at the breakpoit)
> px @ rbp-0xc
01
```
Answer: 1<br>
4. What operator is used to change the value of var_8h, input the symbol as your answer(symbols include +, -, *, /, &, |):
```
 0x560e7c1d0623      8365f864       and dword [var_8h], 0x64
```
Answer: &

## Task 5 - Loops

1. What is the value of var_8h on the second iteration of the loop?
```
> db jle
> dc 
> dc 
> dc (it seems a for loop, which tests first, so need to reach the comparison 3 times)
> px @ rbp-0x8
5
```
Answer: 5 <br>
2. What is the value of var_ch on the second iteration of the loop?
```
> (same as before)
> px @ rbp-0xc
0
```
Answer: 0 <br>
3. What is the value of var_8h at the end of the program?
```
> db pop rbp
> px @ rbp-0x8
2
```
Answer: 2 <br>
4. What is the value of var_ch at the end of the program?
```
> db pop rbp
> px @ rbp-0xc
0
```
Answer: 0<br>

## Task 6 - crackme1
The program starts by loading 127 to var_40h
```
0x564bc92f283c      488d057c0100.  lea rax, [0x564bc92f29bf] ; "127"
0x564bc92f2843      488945c0       mov qword [var_40h], rax
```
And then procceeds by extracting tokens from strings with "." as the delimiter for the slicing, using the function strtok() <br>

Just checking the strings it uses in the strtok, is possible to identify the answer
```
01.\u7257\u6e6f\ (...)
```

Accepted answer: 127.0.0.1 <br>

Note: this exercise also gives correct password with other values, which is stupid (ex: 127; 127.0; etc.)

## Task 7 - crackme2
The program loads the password from a file, with password size 11 <br>
Reading the file directly gives us the string <b> vs3curepwd </b> which is not accepted <br>

Checking the behaviour we can see the following <br>

a) Loads 9 @ rbp-0x44 (mov dword [var_44h], 9) <br>
b) Loads 0 @ rbp-0x40 (mov dword [var_40h], 0) <br>
c) Loads "s3curepwd" @ rbp-0x2e (read from file, manually checked) <br>
d) Inserted password @ rbp-0x23 (manually checked) <br>

Compare a) and 0 (cmp dword [var_44h], 0) <br>
Jumps to "Correct password" if <= 0 (JLE) <br>

Otherwise:
- copy b) to eax and sign-extends it to RBX 
  - mov eax, dword [var_40h]
  - movsxd rbx, eax
- loads d) address and calculates its lenght 
  - lea rax, [var_23h]
  - (..) 
  - call sym.imp.strlen
- Checks if the length is > 0 () 
  - mov eax, dword [var_40h]
  - movsxd rbx, eax
  - (..) strlen here
  - cmp rbx, rax
- If bigger jumps to other portion
  - jb 0x557d76351899
- In that portion
  - Copies a) current value to eax and sign-extends it to RAX
  - Copies 1 byte of c) to edx 
    - movzx edx, byte [rbp + rax - 0x2e]
  - Copies b) current value to eax and sign-extends it to RAX
  - Copies 1 byte of d) to eax 
    - movzx eax, byte [rbp + rax - 0x23]
  - Compares the low-order bits of EAX and EDX
    - cmp dl, al

This behaviour basically compares the strings in reverse order meaning:
- Loaded password: 0-N
- Input password: N-0

To get the password reverse the one present in the file but ignoring the first character <br>

<b> Answer </b>:dwperuc3s




