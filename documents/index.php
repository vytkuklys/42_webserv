<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="icon" type="image/svg+hml" href="images/icon.svg"/>
    <link rel="stylesheet" href="style.css">
    <title>Index Page</title>
    <style>
        .hidden{
            display: none;
        }
        .padding-left{
            /* padding-left: .3rem; */
        }
    </style>
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
            function print_files($fileList, $className, $img)
            {
                foreach($fileList as $filename){
                    if(is_file($filename)){
                        echo "<tr class='$className'>";
                        echo '<td class="padding-left"><img src='.$img.'><a href='.$filename.'>', $filename, '</a></td>';
                        echo '<td>'.formatBytes(filesize($filename)).'</td>';
                        echo '<td>'.date("F d Y H:i:s", filemtime($filename)).'</td>';
                        echo "</tr>";
                    }
                }
            }
            function print_dir($fileList, $className)
            {
                foreach($fileList as $filename){
                    if(is_dir($filename)){
                        echo "<tr class='$className'>";
                        echo '<td><img src="../images/dir.svg"><span href='.$filename.' class="toggler">', $filename, '</span></td>';
                        echo '<td>'.formatBytes(filesize($filename)).'</td>';
                        echo '<td>'.date("F d Y H:i:s", filemtime($filename)).'</td>';
                        echo "</tr>";
                        $subDir = glob($filename."/*");
                        print_files($subDir, "hidden"." ".$filename, "../images/sub_file.svg");
                        print_dir($subDir, "hidden"." ".$filename);
                    }
                }
            }
            echo "<table>";
            echo "<tr><th>Filename</th><th>Size</th><th>Last Modified</th></tr>";
            $fileList = glob('*');
            print_dir($fileList, "");
            print_files($fileList, "", "../images/file.svg");
            echo "</table>";
        ?>
    </div>
    <script>
        const toggler = document.querySelectorAll('.toggler');
        toggler.forEach(item => {
            item.addEventListener('click', () =>{
                const name = item.textContent;
                const hidden = document.querySelectorAll(`.${name}`);
                hidden.forEach(hidden_it =>{
                    hidden_it.classList.toggle('hidden');
                })
            })
        })
    </script>
</body>
</html>