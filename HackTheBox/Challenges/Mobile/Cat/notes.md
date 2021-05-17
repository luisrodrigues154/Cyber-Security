# Cat

This challenge was pretty simple, didn't have to write notes to remember stuff because it was as simple as a 3 step challenge <br>

First (after the challenge's .zip extraction) it produced a **cat.ab** file which the linux file program indicates as an **Android Backup** file 

```bash
cmd: file cat.ab
cat.ab: Android Backup, version 5, Compressed, Not-Encrypted
```

Next thing, google it !! I literally just searched for ".ab file" and the first it was **"How to extract (...)"**<br>

Yeah, why not xD! It just used a bash printf and the tail command to:
1. with the printf, create a valid .tar.gz file header
2. with the tail, get the actual content (it is possible in this case because it is not encrypted and the extraction can be done)
3. pipe the new file to tar and decompress it

```bash
cmd: (printf "\x1f\x8b\x08\x00\x00\x00\x00\x00" ; tail -c +25 cat.ab ) |  tar xfvz -
cmd: ls -la
drwxr-xr-x 50 waza waza 4096 May 17 15:52 apps
drwxr-xr-x  3 waza waza 4096 May 17 15:52 shared

cmd (pwd: (..)/shared):ls -la *

Pictures:
total 4128
drwxr-x---  2 waza waza    4096 May 17 15:52 .
drwxr-xr-x 11 waza waza    4096 May 17 15:52 ..
-rw-r-----  1 waza waza  471362 May 17 15:52 IMAG0001.jpg
-rw-r-----  1 waza waza  487069 May 17 15:52 IMAG0002.jpg
-rw-r-----  1 waza waza  407861 May 17 15:52 IMAG0003.jpg
-rw-r-----  1 waza waza 2046587 May 17 15:52 IMAG0004.jpg
-rw-r-----  1 waza waza  283402 May 17 15:52 IMAG0005.jpg
-rw-r-----  1 waza waza  511006 May 17 15:52 IMAG0006.jpg
```

That was the only content of the shared folder, checking the images there's a bunch of cats and a Mr.tricky moves holding a "Top secret" document which has the flag at the bottom... <br>
I laughed hard with this tricky moves ahah

## Flag: HTB{ThisBackupIsUnprotected}


