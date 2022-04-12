#!./php-cgi

<?php
	// pcntl_signal(SIGINT,SIG_IGN, false);
	if (isset($_FILES['user_file']) && ($_SERVER['REQUEST_METHOD'] == "POST"))
		move_uploaded_file($_FILES['user_file']['tmp_name'], '../documents/uploaded/' . $_FILES['user_file']['name']);
	else
	{
		$fd = fopen('php://stdin', 'rb');
		if ($fd === FALSE)
		{
			echo "error input\n";
			phpinfo(INFO_VARIABLES | INFO_ENVIRONMENT);
			exit("Failed to open stream to URL");
		}
		$file_name = "../documents/uploaded/uploaded";
		$file_error = fopen("../documents/uploaded/error", 'w');;
		$i = 0;
		while(file_exists($file_name))
		{
			$file_name = "../documents/uploaded/uploaded" . $i;
			$i++;
		}
		$out = fopen($file_name, 'w');
		// fwrite($out, "=====done======\n", 17);

		while(!feof($fd))
		{
			$line = '';
			// fwrite($out, "\n\n===bege========", 17);
			if($line = fread($fd, 8192/2) === FALSE)
				fwrite($file_error, "\n\n===end========", 16);
			if(fwrite($out, $line, 10000) == false)
				break;
			// fwrite($out, "\n\n===done========", 17);
		}

		// fwrite($out, "\n\n===end========", 16);
		fclose($out);
		fclose($fd);
	}
?>