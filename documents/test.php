#!/usr/bin/php

<?php

parse_str($argv[4], $_HEAD);
// foreach( $_HEAD as $variableName ) {
//     fwrite(STDOUT, $variableName);
//     fwrite(STDOUT, "\r\n");
// }

$line = trim(fgets(STDIN));
fwrite(STDOUT, $line);
$pos = strpos($_HEAD['Content-Type'], "boundary");
if($pos === false)
    echo "tests\n";
$test = trim(substr($_HEAD['Content-Type'], $pos + strlen("boundary=")), "- \r");
echo $test . "\n";
echo $_HEAD['Content-Length'];
$byts = intval($_HEAD['Content-Length']);
$stream = fopen("./test1.txt", "w");
while($byts > 0)
{
    // echo $byts . "\n";
    $test1 = fread(STDIN, $byts);
    $byts -= fwrite($stream, $test1);
    echo $byts .  " = " . strlen($test1) . "\n";
    if ($byts < 10000)
    {
        echo ">" . substr($test, 0) . "<\n";
        // echo ">" . (trim(substr($test1,strlen($test1) - 35), "- \r"), "<\n";
        $pos = strpos($test1, $test);
        if ( $pos === false)
        {
            continue;
        }
        else
            break;

    }
}
fwrite(STDOUT, "end of php script\n");
fclose($stream);



?>

