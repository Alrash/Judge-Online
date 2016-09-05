<?php
/* *
 * Author: Alrash
 * build os: Archlinux
 * create time: 2016-05-01
 * thanks for: yeszao(详见特别鸣谢部分)
 * 由于一些特殊的原因，本系统php版本为5.3.x，未用php7的特性
 */
session_start();

/* *
 * 检测响应头语言设置，返回系统默认语言
 * 返回值有两种，为中文，返回中文简体，否则返回英语
 */
function getSystemLanguage()
{
    if (preg_match("/zh-../", $_SERVER['HTTP_ACCEPT_LANGUAGE']))
        return 'zh_CN';
    else
        return 'en';
}

// 开启调试模式
define('APP_DEBUG', true);

//定义生存时间
defined('LIFTTIME') or define('LIFTTIME', 3600 * 24 * 365);

//定义语言数组常量
define('LANGUAGE',"return array('zh_CN', 'en');");

/* *
 * 设置语言cookie值
 * 条件：不存在$_COOKIE['LANG']，或值不是已有语言
 */
if (!isset($_COOKIE['LANG']) || !in_array($_COOKIE['LANG'], eval(LANGUAGE)))
{
    setcookie('LANG', getSystemLanguage(), time() + LIFTTIME, '/');
    $_COOKIE['LANG'] = null;
}

// 应用目录为当前目录
define('APP_PATH', __DIR__.'/');

// 加载框架
require './fastphp/FastPHP.php'; 