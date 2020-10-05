# BabyRE challenge

## 1 - Initial 

Check the file type
```
ELF 64-bit LSB shared object, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=25adc53b89f781335a27bf1b81f5c4cb74581022, for GNU/Linux 3.2.0, not stripped
```
Nice, we got simbols! <br>

Running strings a message in the .data section shows a funny string:

```
Dont run `strings` on this challenge, that is not the way!!!!
```

Also there is a string with "abcde122313" after "Insert key:" which make me believe this is the password <br>

Another interesting thing is that the flag is in the strings format but splited and the regex to reconstruct it also appears...

```
HTB{B4BYH
_R3V_TH4H
TS_Ef
[]A\A]A^A_
```

To speed this up, lets just static analyze it

## 2 - Fast analysis

just dissassemble this and check for interesting things with objdump

```
objdump -d -M intel baby
```

The most interesting thing is a string being loaded to source puts arguments... 

## 3 - Run the program

Lets run and test the suspicious password:

```
Insert key: abcde122313
HTB{B4BY_R3V_TH4TS_EZ}
```

