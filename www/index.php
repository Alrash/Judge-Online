<?php
session_start();
// 应用目录为当前目录
define('APP_PATH', __DIR__.'/');

// 开启调试模式
define('APP_DEBUG', true);

//定义生存时间
defined("LIFTTIME") or define("LIFTTIME", 3600 * 24 * 365);

// 加载框架
require './fastphp/FastPHP.php'; 