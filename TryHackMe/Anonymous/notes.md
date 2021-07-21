# Anonymous


# Solutions

## 1 - Enumerate the machine.  How many ports are open?

Lets do an nmapy boy :)

```bash
21/tcp  open  ftp         vsftpd 2.0.8 or later                                                                                                                         
| ftp-anon: Anonymous FTP login allowed (FTP code 230)                                                                                                                  
|_drwxrwxrwx    2 111      113          4096 Jun 04  2020 scripts [NSE: writeable]                                                                                      
| ftp-syst:                                                                                                                                                             
|   STAT:                                                                                                                                                               
| FTP server status:                                                                                                                                                    
|      Connected to ::ffff:10.9.128.84                                                                                                                                  
|      Logged in as ftp                                                                                                                                                 
|      TYPE: ASCII                                                                                                                                                      
|      No session bandwidth limit
|      Session timeout in seconds is 300
|      Control connection is plain text
|      Data connections will be plain text 
|      At session startup, client count was 4
|      vsFTPd 3.0.3 - secure, fast, stable 
|_End of status
22/tcp  open  ssh         OpenSSH 7.6p1 Ubuntu 4ubuntu0.3 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   2048 8b:ca:21:62:1c:2b:23:fa:6b:c6:1f:a8:13:fe:1c:68 (RSA)
|   256 95:89:a4:12:e2:e6:ab:90:5d:45:19:ff:41:5f:74:ce (ECDSA)
|_  256 e1:2a:96:a4:ea:8f:68:8f:cc:74:b8:f0:28:72:70:cd (ED25519)
139/tcp open  netbios-ssn Samba smbd 3.X - 4.X (workgroup: WORKGROUP)
445/tcp open  netbios-ssn Samba smbd 4.7.6-Ubuntu (workgroup: WORKGROUP)
```

Nmap also had smb shares references, might come handy later

**Answer:** 4

## 2 - What service is running on port 21?

**Answer:** ftp

## 3 - What service is running on ports 139 and 445?

**Answer:** smb

## 4 - There's a share on the user's computer.  What's it called?

With enumeration over the share, we got its name. For this i used enum4linux but we could've enumerated it manually

```bash
Sharename       Type      Comment                                                                                                   
---------       ----      -------                                                                                                   
print$          Disk      Printer Drivers                                                                                           
pics            Disk      My SMB Share Directory for Pics                                                                           
IPC$            IPC       IPC Service (anonymous server (Samba, Ubuntu))
```

**Answer:** pics

## 5 - user.txt

After poking around with enum4linux and crackmapexec i ended up finding that we can actually overwrite the script found in the FTP's scripts folder because we have write permissions for it

```bash
-rwxr-xrwx    1 1000     1000          314 Jun 04  2020 clean.sh
```

Just for confirmation, i uploaded a bash script that pings me 3 times

```bash
#!/bin/bash
ping -c 3 10.9.128.84
```

And we got it :D, lets revshell it

```bash
# cmd: whoami && id
namelessone
uid=1000(namelessone) gid=1000(namelessone) groups=1000(namelessone),4(adm),24(cdrom),27(sudo),30(dip),46(plugdev),108(lxd)

# cmd: cat user.txt
90d6f992585815ff991e68748c414740
```

**Answer:** 90d6f992585815ff991e68748c414740

## 6 - root.txt

The user have very interesting groups tho, more specifically that **lxd** one. Why? check [this](https://book.hacktricks.xyz/linux-unix/privilege-escalation/interesting-groups-linux-pe/lxd-privilege-escalation) out :p

Annnnnd here we go

```bash
# cmd: whoami && id
root
uid=0(root) gid=0(root)

# cmd: cat /mnt/root/root/root.txt
4d930091c31a622a7ed10f27999af363
```

**Answer:** 4d930091c31a622a7ed10f27999af363

