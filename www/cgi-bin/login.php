#!/usr/bin/php
<?php

$body = trim(file_get_contents("../.input"));

function saveAccount($body) {

    $file = '.cookie';
    if (!file_exists($file)) { fopen($file, "w"); }

    if (filesize($file) == 0)
        { $body = $body . PHP_EOL; file_put_contents($file, $body, FILE_APPEND); echo "ACCOUNT SAVED"; exit(); }

    $content = file_get_contents($file);

    $array = explode(PHP_EOL, $content);

    $i = 0;
    while ($i < count($array))
    {
		$saved = explode('&', $array[$i]);
		$new = explode('&', $body);
        if ($array[$i] === $body) { echo "LOGIN"; exit(); }
		else if ($saved[0] === $new[0] && $saved[1] !== $new[1])
			{ echo "WRONG PARAMETER"; exit(); }
        $i++;
    }
    $body = $body . PHP_EOL;
    file_put_contents($file, $body, FILE_APPEND);
    echo "ACCOUNT SAVED";
}

saveAccount($body);

?>