#!/usr/bin/php
<?php

$body = trim(file_get_contents("../../.input"));

function deleteFromFile($body) {
    $file = 'database.txt';
    
    if (file_exists($file)) {

        $body = str_replace("\"", '', $body);
        $content = file_get_contents($file);
        $content = str_replace(array("\n"), ' ', $content);

        $array = explode(' ', $content);

        if (array_search($body, $array) === false) { echo "Elemento non presente in $file"; exit(); }
        
        $i = 0;
        while ($i < count($array))
        {
            if ($array[$i] == $body) { unset($array[$i]); break; }
            $i++;
        }

        $content = implode(' ', $array);

        $content = str_replace(' ', PHP_EOL, $content);
        file_put_contents($file, $content);

    } else { echo "File non trovato: $file"; }
}

deleteFromFile($body);

?>
