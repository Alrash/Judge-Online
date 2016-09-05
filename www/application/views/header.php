<?php
    $languagePath = isset($_COOKIE['LANG']) ? $_COOKIE['LANG'] : getSystemLanguage();
    require_once STATIC_PATH . "language/$languagePath/language.php";
?>
<!DOCTYPE html>
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
