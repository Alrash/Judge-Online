<?php
/**
 * Author: Alrash
 * 处理字符串的专用文件
 */

define('USUAL_CHARS', 'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz');
define('SPECIAL_CHARS', "!@#$%^&*()-+=/{}|':;?.\\,");

/**
 * 获得含有[a-zA-Z0-9]字符的长度在[min, max]之间的字符串
 */
function getUsualRandString($minLength, $maxLength)
{
    $str = "";
    $chars = constant('USUAL_CHARS');
    $maxLen = strlen($chars) - 1;
    $len = rand($minLength, $maxLength);

    for ($i = 0; $i < $len; $i++)
        $str .= $chars[rand(0, $maxLen)];

    return $str;
}

/**
 * 获得含有特殊字符的字符串
 */
function getSpecialRandString($minLength, $maxLength)
{
    $str = "";
    $chars = constant('USUAL_CHARS') . constant('SPECIAL_CHARS');
    $maxLen = strlen($chars) - 1;
    $len = rand($minLength, $maxLength);

    for ($i = 0; $i < $len; $i++)
        $str .= $chars[rand(0, $maxLen)];

    return $str;
}

/**
 * 去除字符串首尾空格
 * 转换特殊字符，如<>，转换成html专用的
 * 取消反斜线
 */
function modifyStringItem($data)
{
    return htmlspecialchars(stripcslashes(trim($data)));
    /*$data = trim($data);                //blank
    $data = stripslashes($data);
    $data = htmlspecialchars($data);    //prevent script

    return $data;*/
}

function htmlReturn($item)
{
    return preg_replace('/\/r\/n|\/n/' , '<br>', $item);
}

function htmlBlank($item)
{
    return preg_replace('/ /', '&nbsp', preg_replace('/\t/', '    ', $item));
}

/**
 * 为字符串添加引号
 */
function addQuotes($String)
{
    return "\"" . $String . "\"";
}
?>