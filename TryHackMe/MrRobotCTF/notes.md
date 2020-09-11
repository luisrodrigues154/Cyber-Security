# MrRobot CTF

## 1 - What is key 1?

Issue a nmap

```
nmap -sC -sV $IP -oN nmap/initial

22/tcp  closed ssh
80/tcp  open   http     Apache httpd
|_http-server-header: Apache
|_http-title: Site doesn't have a title (text/html).
443/tcp open   ssl/http Apache httpd
|_http-server-header: Apache
|_http-title: Site doesn't have a title (text/html).
| ssl-cert: Subject: commonName=www.example.com
| Not valid before: 2015-09-16T10:45:03
|_Not valid after:  2025-09-13T10:45:03
```

Has a server, issue a gobuster with medium wordlist and visit to check

```
gobuster dir -u $IP -w /usr/share/wordlists/dirbuster/directory-list-2.3-medium.txt -o gobuster/medium

/images (Status: 301)
/blog (Status: 301)
/sitemap (Status: 200) <- gives XML parsing error
/rss (Status: 301)
/login (Status: 302)
/video (Status: 301)
/feed (Status: 301)
/0 (Status: 301)
/image (Status: 301)
/atom (Status: 301)
/wp-content (Status: 301) (WORDPRESS blog)
/admin (Status: 301)
/audio (Status: 301)
/intro (Status: 200)
/wp-login (Status: 200)
/css (Status: 301)
/rss2 (Status: 301)
/license (Status: 200)
```
The admin directory keeps manipulating the page through javascript, i'll issue another gobuster in the admin

```
nothing more than the previous
```


After being stuck, checked the hint, suggesting robots.txt

I dont know why neither nmap, neither gobuster found the robots.txt file

The /robots.txt has two files

```
fsocity.dic
key-1-of-3.txt
```

### Answer: 073403c8a58a1f80d943455fb30724b9

## 2 - What is key 2?

The fsocity.dic seems like a brute forcing worlist for paths because it includes var, styles, http

used it with gobuster to verify, nothing interesting found.

i knew it was a wordlist but didnt knew for what so i went 

## 3 - What is key 3?
