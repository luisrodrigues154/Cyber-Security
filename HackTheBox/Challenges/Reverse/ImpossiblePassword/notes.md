# Impossible Password

This is a challenge, NO IP

## 1- Zip file

It is protected with password: hackthebox <br>

## 2- Strings command

if we perform the strings command one string stands out

```
SuperSeKretKey
```

Pasting that in the binary leads us to a second layer of security.

## 3 - Quick analysis

Issuing objdump i can see some hex values (resembling chars) being pushed into a buffer
```
0x41
0x5d
0x4b
0x72
0x3d
0x39
0x6b
0x30
0x3d
0x30
0x6f
0x30
0x3b
0x6b
0x31
0x3f
0x6b
0x38
0x31 
0x74
```

After converting this to ascii i got the following

```
A]Kr=9k0=0o0;k1?k81t
```

It did not worked as the second layer security password.

## 4 - Static analysis

Using ghidra we can see that the program (in the second verification) generates a random string that is compared to the sourced.<br>
If they are the same then the program will generate the flag using the buffer seen at the previous step as a key to derive it. <br>

Since it uses a JNZ to test the result, we can patch this binary securily by setting the instruction from JNZ to JZ and we can just source some value that evaluates to false.
<br>

To patch this i used a script named SavePatch and just source the instruction address and size 1 byte and it worked flawlessly.

```
FLAG: HTB{40b949f92b86b18}
```



