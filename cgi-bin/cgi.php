#!./php-cgi

<?php
	// pcntl_signal(SIGINT,SIG_IGN, false);
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
		// $file_name = "/goinfre/shackbei/upploads/test";
		$file_name = $_SERVER['DOCUMENT_ROOT'];
		// if ($_SERVER['REQUEST_METHOD'] == "POST")
		// {
			$i = rand();
			$file_name .= $i;
			while(file_exists($file_name))
			{
				// fwrite($file_error, $i);

				$file_name = $_SERVER['DOCUMENT_ROOT'] . $i;
				$i++;
			}
		// }
		// $file_error = fopen("/Users/shackbei/Documents/code/Projects/webserv/documents/uploaded/error", 'w');
		// fwrite($file_error, $file_name);
		// phpinfo(INFO_VARIABLES | INFO_ENVIRONMENT);

		$out = fopen($file_name, 'w');
		if ($out === FALSE)
		{
			echo "error input\n";
			phpinfo(INFO_VARIABLES | INFO_ENVIRONMENT);
			exit("Failed to open out");
		}
		$line = '';
		// while(!feof($fd))
		while(1)
		{
			// fwrite($out, "\n\n===fread========", 16);
			if(($line = fread($fd, 8192)) === false)
			{
				// fwrite($file_error, "\n\n===end========", 16);
				break;
			}
			// fwrite($out, "\n\n===fwrite========", 16);
			if((fwrite($out, $line, 8192)) === FALSE)
			{
				// fwrite($file_error, "\n\n===end========", 16);
				break;
			}
			if(strlen($line) == 0)
				break;
		}
		// fwrite($out, "\n\n===end========", 16);
		fclose($out);
		fclose($fd);
		fclose($file_error);
	}

