<?php
/**
 * Author:Alrash
 * 用于改变默认语言
 */
if (!isset($_SESSION))
    session_start();

$lang = "_zh_cn";
if (preg_match("/zh-../", $_SERVER['HTTP_ACCEPT_LANGUAGE']))
    $lang = "_zh_cn";
else
    $lang = "_en";

if (isset($_SESSION['lang']))
    $lang = $_SESSION['lang'];

$_SESSION['lang'] = $lang;
//$lang = "_en";
?>
