# Bypass

Since this is an .exe (windows) program and cant run it on linux, i'll just go straight to ghidra and analyze it

## 1 - static analysis

Using dnspy and assuming that the function 0() is the starting point, we can deduce that at 0.2() the flag is printed using

```
5.5 + 0.2 + 5.6
HTB{ +     + }
```

Running with debugging and setting breakpoints at the conditions, it is possible to change the flag's values to true and print the flag with this approach.

```
FLAG: HTB{SuP3rC00lFL4g}
```