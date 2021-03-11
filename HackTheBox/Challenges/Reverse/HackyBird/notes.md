# HackyBird

Starting by checking the file type we can see that it's a windows 32bit executable
``` 
PE32 executable (GUI) Intel 80386, for MS Windows
```

Before checking what it actually is, running strings return some things but not the flag so i assume that it must be shown in a form of an asset (image) within the application <br>

Passing it to a windows VM it is just a windows game resembling the flappy bird (no shi sherlock! xD) <br>


It works like the original game, no surprises! Lets check if the assets theory is in fact true by using 7-zip to extract the .exe<br>

But no, there is no image containing the flag!! <br>

The **.exe** is stripped so its a bit rough to read, the strings in the game are not present in the strings but i noticed that the name of the window (**flappy101**) is set sub_403570