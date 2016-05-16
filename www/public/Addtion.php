<?php
/**
 * Author: Alrash
 * 用于输出<head>标签内的内容
 */

echo '<meta content="text/html">';
echo '<meta charset="utf-8">';
echo '<meta name="author" content="Alrash">';

//图标
echo '<!--<link href="favicon_32*32.ico" rel="icon">-->';

//jquery解析库
echo '<!--<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.8.0/jquery.min.js"></script>-->';
echo '<script type="text/javascript" src="http://code.jquery.com/jquery-2.1.1.min.js"></script>';

//js ase加密脚本
//前三行为从google导入，但因为大部分人并没有翻墙工具，于是使用了github上第三方重新封装的第三方库
echo '<!--<script src="http://crypto-js.googlecode.com/svn/tags/3.1.2/build/rollups/aes.js"></script>';
echo '<script src="http://crypto-js.googlecode.com/svn/tags/3.1.2/build/rollups/md5.js"></script>';
echo '<script src="http://crypto-js.googlecode.com/svn/tags/3.1.2/build/components/pad-zeropadding.js"></script>-->';
echo '<script type="text/javascript" src="/js/CryptoJS/rollups/aes.js"></script>';
echo '<script type="text/javascript" src="/js/CryptoJS/rollups/md5.js"></script>';
echo '<script type="text/javascript" src="/js/CryptoJS/components/pad-zeropadding-min.js"></script>';
    
//css样式脚本
echo '<link rel="stylesheet" type="text/css" href="/css/JudgeOnline.css">';

//自己的js脚本
echo '<script type="text/javascript" src="/js/Menu.js"></script>';
