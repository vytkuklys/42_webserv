<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="icon" type="image/svg+hml" href="icon.svg"/>
    <link rel="stylesheet" href="style.css">
    <title>Index Page</title>
</head>
<body>
    <div>
    <ul>
        <li><a href="/">Home</a></li>
        <li><a href="/upload.html">Upload</a></li>
        <li><a href="/index.php" class="current">Index</a></li>
    </ul>
        <p>Index Page</p>

        <?php
            function formatBytes($bytes, $precision = 2) {
                $units = array('B', 'KB', 'MB', 'GB', 'TB');

                $bytes = max($bytes, 0);
                $pow = floor(($bytes ? log($bytes) : 0) / log(1024));
                $pow = min($pow, count($units) - 1);
                return round($bytes, $precision).' '.$units[$pow];
            }
            echo "<table>";
            echo "<tr><th>Filename</th><th>Size</th><th>Last Modified</th></tr>";
            $fileList = glob('*');
            foreach($fileList as $filename){
                if(is_file($filename)){
                    echo "<tr>";
                    echo '<td><a href='.$filename.'>', $filename, '</a></td>';
                    echo '<td>'.formatBytes(filesize($filename)).'</td>';
                    echo '<td>'.date("F d Y H:i:s", filemtime($filename)).'</td>';
                    echo "</tr>";
                }
            }
            echo "</table>";
        ?>
    </div>
</body>
</html>