<?php
    session_start();
    require_once("./script/config/lang.php");
    require_once("./script/language.php");
?>
<!DOCTYPE html>
<html>
<head>
    <?php require_once('./script/Head.php'); ?>
    <script src="time.js"></script>
    
    <title><?php echo $title['index' . $lang]?></title>
</head>
<body class="index" onload="clock()">
    <?php require_once "./script/Menu.php";//导入导航栏?>
    
    <div id="Time" style="text-align: center;font-size: 20px;"></div>
    <div class="search">
        <i id="isearch"></i>
        <input id="search" placeholder="<?php echo $search['search' . $lang]?>">
        <button id="bsearch"><?php echo $button['search' . $lang]?></button>
    </div>
<?php
    foreach ($_SESSION as $key=> $value)
        echo $key . " " . $value . "#<br>";
?>
</body>
</html>