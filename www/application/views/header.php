<?php
if (true)
    require_once STATIC_PATH . 'language/zh_CN/language.php';
else
    require_once STATIC_PATH . 'language/en/language.php';
?>
<!DOCTYPE HTML>
<html>
<head>
    <?php
    require_once STATIC_PATH . 'Addtion.php';
    ?>
    <title><?php echo $title[$Title]?></title>
</head>
<body class="<?php echo $bodyStyle;?>">
<?php
require_once STATIC_PATH . 'Menu.php';
?>
