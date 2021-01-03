# Dobby 

dobby needs to be root to help harry potter, dobby needs to be a free elf <br>

Difficult: Easy

# Solution

Lets find the machine IP (since i dont know the password for the acc) by scanning the network ! <br>

cmd: nmap -sn 192.168.1.0/24 -oN nmap/devices
```
Nmap scan report for HogWarts.lan (192.168.1.73)
```

Now lets scan this address <br>

export IP=192.168.1.73 <br>

cmd: nmap -sC -sV -A $IP -oN nmap/initial 
```
80/tcp open  http    Apache httpd 2.4.46 ((Ubuntu))
|_http-server-header: Apache/2.4.46 (Ubuntu)
|_http-title: Draco:dG9vIGVhc3kgbm8/IFBvdHRlcg==
```

The http-title looks like some creds... lets decode what looks like the password (??) in b64 
```
too easy no? Potter
```

Fuzzing the webserver gives back some interesting results <br>

cmd: gobuster dir -u http://$IP -w /opt/SecLists/Discovery/Web-Content/big.txt -o fuzz/big.log -x php,txt,html -t 80
```
/index.html (Status: 200)      
/log (Status: 200)              
/server-status (Status: 403)    
/phpinfo.php (Status: 200)                       
```

Checking <b>/log </b> we got
```
pass:OjppbGlrZXNvY2tz

hint --> /DiagonAlley
```

Visiting <b> /DiagonAlley </b> it is visible it is a blog running WordPress, lets run a wpscan 
```

```