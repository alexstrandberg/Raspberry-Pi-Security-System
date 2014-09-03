<!DOCTYPE html>
<html>
<head>
<title>Raspberry Pi Security System</title>
</head>
<body>
<h1 style="color:red">Raspberry Pi Security System</h1>
<h2>Pi Camera Control</h2>
<a href="latestvideo.php">Latest Video</a><br/>
<?php
    // shell_exec('sudo -S raspistill -vf -hf -o "/var/www/camera/' . date('Y-m-d H:i:s') . '.jpg"');
    
    if (!empty($_GET['servoleft'])) {
        $ourFileName = "servoleft.txt";
        $ourFileHandle = fopen($ourFileName, 'w') or die("can't open file");
        fclose($ourFileHandle);
        $_GET['takepicture'] = 1;
        sleep(3);
    }
    
    else if (!empty($_GET['servoright'])) {
        $ourFileName = "servoright.txt";
        $ourFileHandle = fopen($ourFileName, 'w') or die("can't open file");
        fclose($ourFileHandle);    
        $_GET['takepicture'] = 1;
        sleep(3);
    }
    
    if (!empty($_GET['takepicture'])) {
        $ourFileName = "takepicture.txt";
        $ourFileHandle = fopen($ourFileName, 'w') or die("can't open file");
        fclose($ourFileHandle);
        sleep(10);
    }
    
    foreach (glob('camera/*.jpg') as $f) {
	    # store the image name
	    $list[] = $f;
	}
	
	sort($list);                    # sort is oldest to newest,
	
	$filename = array_pop($list);
    echo '<br/>Filename: ' . $filename . '<br/>';
	echo '<img src="' . $filename . '" width="648" height="486"/>';
    
    echo '<br/>';
    echo '<input type="submit" value="SERVO LEFT" style="height:100px; width:145px" onclick="window.location = \'livefeed.php?servoleft=1\'" />';
    echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
    echo '<input type="submit" value="TAKE PICTURE" style="height:100px; width:145px" onclick="window.location = \'livefeed.php?takepicture=1\'" />';
    echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
    echo '<input type="submit" value="SERVO RIGHT" style="height:100px; width:145px" onclick="window.location = \'livefeed.php?servoright=1\'" />';
    echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
    echo '<input type="submit" value="RESET" style="height:100px; width:145px" onclick="window.location = \'livefeed.php\'" />';
    
    echo '<br/><br/><a href="camera/">Archive</a>';
?>
</body>
</html>