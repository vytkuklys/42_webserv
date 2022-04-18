#!./php-cgi

<?php
	if (isset($_FILES['user_file']) && ($_SERVER['REQUEST_METHOD'] == "POST") && !isset($_ENV['HTTP_COOKIE']))
		move_uploaded_file($_FILES['user_file']['tmp_name'], '../documents/uploaded/' . $_FILES['user_file']['name']);
	else if(!isset($_ENV['HTTP_COOKIE']))
	{
		$fd = fopen('php://stdin', 'rb');
		if ($fd === FALSE)
		{
			echo "error input\n";
			phpinfo(INFO_VARIABLES | INFO_ENVIRONMENT);
			exit("Failed to open stream to URL");
		}
		$file_name = $_SERVER['DOCUMENT_ROOT'];
		// if ($_SERVER['REQUEST_METHOD'] == "POST")
		// {
			$i = rand();
			$file_name .= $i;
			while(file_exists($file_name))
			{
				$file_name = $_SERVER['DOCUMENT_ROOT'] . $i;
				$i++;
			}
		// }

		$out = fopen($file_name, 'w');
		if ($out === FALSE)
		{
			echo "error input\n";
			phpinfo(INFO_VARIABLES | INFO_ENVIRONMENT);
			exit("Failed to open out");
		}
		$line = '';
		while(1)
		{
			if(($line = fread($fd, 8192)) === false)
			{
				break;
			}
			if((fwrite($out, $line, 8192)) === FALSE)
			{
				break;
			}
			if(strlen($line) == 0)
				break;
		}
		fclose($out);
		fclose($fd);
		fclose($file_error);
	}

