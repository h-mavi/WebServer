#!/usr/bin/php
<?php

function readFromFile() {
    $file = 'database.txt';
    
    if (file_exists($file)) {

        $content = file_get_contents($file);        
        $content = str_replace(array("\n"), ' ', $content);
        $words = explode(' ', $content);

        if ($words[0] == "")  { echo json_encode(["error" => "File vuoto"]);; exit(); }
        
        echo json_encode($words);

    } else { echo json_encode(["error" => "File non trovato: $file"]); }
}

readFromFile();

?>
