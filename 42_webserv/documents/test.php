#!/usr/bin/php

<?php

    // var_dump($HTTP_RAW_POST_DATA);
    if (($stdin = fopen('php://stdin', 'r')) == false)
        echo "fehler open stdin";
    if (($stdout = fopen('php://stdout', 'w')) == false)
            echo "fehler open out";
    while($input = fgets($stdin))
    {
        echo $input . "\r\n";
    }
    if ($input == false)
        echo "input false" . "\r\n";

    var_dump($_POST);
    // phpinfo();

?>