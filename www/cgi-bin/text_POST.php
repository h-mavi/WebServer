#!/usr/bin/php
<?php

$body = trim(file_get_contents("../../.input"));

function saveToFile($body) {
    $file = 'database.txt';

    if (!file_exists($file)) { fopen($file, "w"); }

    $body = str_replace("\"", '', $body);
    $content = file_get_contents($file);        
    $content = str_replace(array("\n"), ' ', $content);
    

    $array = explode(' ', $content);

    $i = 0;
    while ($i < count($array))
    {
        if ($array[$i] == $body) { echo "Elemento già presente in $file"; exit(); }
        $i++;
    }

    $body = str_replace(array("\n"), '', $body);
    
    if (filesize($file) == 0) { $data_to_save = $body; }
    else { $data_to_save = PHP_EOL . $body; }

    file_put_contents($file, $data_to_save, FILE_APPEND);
}

saveToFile($body);

?>
