<!DOCTYPE html>
<html>
<head>
<title>Raspberry Pi Security System</title>
</head>
<body>
<h1 style="color:red">Raspberry Pi Security System</h1>
<h2>Viewing latest incident's footage</h2>
<a href="livefeed.php">Camera Control</a><br/>
<?php 
    foreach (glob('camera/*.mp4') as $f) {
	    # store the image name
	    $list[] = $f;
	}
	
	sort($list);                    # sort is oldest to newest,
	
	$filename = array_pop($list);
    echo '<br/>Filename: ' . $filename . '<br/>';
	echo "<video width='648' height='486' controls autoplay><source src='$filename' type='video/mp4'>Your browser does not support the video tag.</video>";   # Newest
    
    echo '<br/><br/><a href="camera/">Archive</a>';
?>