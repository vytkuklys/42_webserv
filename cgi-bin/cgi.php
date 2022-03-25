<?php
	if (isset($_FILES['user_file']))
		move_uploaded_file($_FILES['user_file']['tmp_name'], '../documents/uploaded/' . $_FILES['user_file']['name']);
	else
	{
		$fd = fopen('php://stdin', 'rb');
		$file_name = "../documents/uploaded/uploaded";
		$i = 0;
		while(file_exists($file_name))
		{
			$file_name = "../documents/uploaded/uploaded" . $i;
			$i++;
		}
		$out = fopen($file_name, 'w');

		if ($fd == false)
		{
			echo "error input\n";
			phpinfo(INFO_VARIABLES | INFO_ENVIRONMENT);
		}
		else
		{
			while(($line = fread($fd, 100)))
			{
				fwrite($out, $line, 100);
			}
		}
	}
?>