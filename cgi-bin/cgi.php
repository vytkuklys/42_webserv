#!/usr/bin/php

<?php
parse_str($argv[4], $_HEAD);
if (isset($_HEAD['Content-Type']))
{
    $pos = strpos($_HEAD['Content-Type'], "boundary");
    if(isset($pos) && $pos === false)
    {
        // echo "no boundary defind\n";
        $tmp = "./documents/uploaded/undefind_name";
        settype($i, "int");
        $i = 1;
        while (file_exists($tmp))
        {
            $tmp = trim($tmp, "0123456789");
            $tmp .= strval($i);
            $i++;
        }
        $stream = fopen($tmp, "w");
    }
    else
    {
        $test = trim(substr($_HEAD['Content-Type'], $pos + strlen("boundary=")), "- \r");
    }

}
if (isset($_HEAD['Content-Length']))
    $byts = intval($_HEAD['Content-Length']);
else
{
    // echo "cunked\n";
    $cunked = true;
    $byts = 0;
}
while(isset($cunked) || $byts > 0 )
{
    if(($line= fgets(STDIN)) == false)
        break;
    $byts -= strlen($line);
    if(isset($test) && strpos($line, $test))
    {
        while(($line[0] != "\r") && ($line[1] != "\n"))
        {
            if(($line= fgets(STDIN)) == false)
                break;
            $byts -= strlen($line);
            if (strpos($line, "filename="))
            {
                $filename = "./documents/uploaded/";
                $filename .= trim(substr($line, strpos($line, "filename=") + strlen("filename=")), "\"\r\n\t");
                $stream = fopen($filename, "w");
            }
        }
    }
    else
    {
        if (isset($stream))
            fwrite($stream, $line);  
    }
}
fclose($stream);

?>