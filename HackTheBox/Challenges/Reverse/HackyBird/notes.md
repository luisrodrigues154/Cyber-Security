# HackyBird

Starting by checking the file type we can see that it's a windows 32bit executable
``` 
PE32 executable (GUI) Intel 80386, for MS Windows
```

Before checking what it actually is, running strings return some things but not the flag so i assume that it must be shown in a form of an asset (image) within the application <br>

Passing it to a windows VM it is just a windows game resembling the flappy bird (no shi sherlock! xD) <br>


It works like the original game, no surprises! Lets check if the assets theory is in fact true by using 7-zip to extract the .exe<br>

But no, there is no image containing the flag!! <br>

The **.exe** is stripped so its a bit rough to read, the strings in the game are not present in the strings but i noticed that a windows **.dll** is called to draw the text onto the screen (**DrawTextW**) which was only called from **sub_4018D0** <br>

In this function we can see that some text is actually being used. This can be affirmed with due to the variables naming (if it isn't, i'll blame IDA xD)
```
v6 = a5;
v7 = lpchText;
v8 = this;
```

In order to check this i used cheat engine to determine the memory address of the score so by injecting a well-known score we can verify that the variable **lpchText** is actually the score. <br>

The only problem now is the debugging because the game is asynchronous and receives callbacks from multiple dll's having multiple execution flows (+6 threads)
```
user32.dll
msimg32.dll
kernel32.dll
gdi32.dll
ntdll.dll
```

After a bit of research, i found out that we can see (with the cheat engine) which accesses are performed to the score memory address. So, watching that memory address, we can see the comparision made when the bird crosses a pipe and thus it reveals the compare that give us the win

```
cmp [esi+0000094], 0003e7
```

This instruction tells us that the program compares, when a pipe is crossed, if the score (at esi+0000094) is equal to 0003e7 which, in decimal, is 999 <br>

To win, changing the score inital value to 999 and cross a pipe give us the flag

## Flag: HTB{game_h3kk1n_is_funsies!}