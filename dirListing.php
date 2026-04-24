#!/usr/bin/php
<?php

$files_name = trim(file_get_contents(".input"));
// $files_name = trim(file_get_contents("php://stdin"));

function dirListing($files_name)
{
    $names = explode(' ', $files_name);

    $response = "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n" .
        "<!DOCTYPE html>\r\n" .
        "<html>\r\n" .
        "<head>\r\n" .
            "<link rel=\"icon\" href=\"favicon.ico\" type=\"image/x-icon\">\r\n" .
            "<title>Listing Directory</title>\r\n" .
        "</head>\r\n" .
        "<body>\r\n" .
            "<main>\r\n" .
                "<h1 style=\"font-family:system-ui;\">Listing Directory</h1>\r\n" . 
                "<p>\r\n" . 
                    "<a href=\"";
            



    
    $i = 1;
    while ($i < count($names))
    {
        if ($i != 1 ) { $response = $response . "<a href=\""; }
        $response = $response . $names[0] . $names[$i] . "\" style=\"font-family:system-ui;padding:0.6rem 1rem;border-radius:6px;background:#F9F5F1;color:#ce7231;text-decoration:none;font-weight:600;\">" . $names[$i] . "</a>\r\n";
        $i++;
    }

    $response = $response . "</p>\r\n" .
                        "</main>\r\n".
                    "</body>\r\n".
                    "</html>";

    echo $response;
}

dirListing($files_name);

?>
