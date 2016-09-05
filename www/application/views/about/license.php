<?php
/**
 * Author: Alrash
 * 用于输出版权信息
 */
echo '<style type="text/css">li{padding:2px 2px;}</style>';
echo '<li>'. $about['right']['license']['author'] . 'Alrash</li>';
echo '<li>' . $about['right']['license']['version'] . 'v1.0</li>';
echo '<li>' . $about['right']['license']['copyleft'] . '</li>';
echo '<li>' . $about['right']['license']['copyleftLeave'] . '</li>';
echo '<li>' . $about['right']['license']['thanks']
          .'<div style="padding:0 2em">'
               . 'FastPHP Framework -- <a href="https://github.com/yeszao/FastPHP" target="_blank">yeszao</a>'
               . '<br> Phpass/PasswordHash.php -- <a href="http://www.openwall.com/phpass/" target="_blank">Openwall</a>'
               . '<br> How to use ptrace -- <a href="http://www.cnblogs.com/tangr206/articles/3094358.html" target="_blank">tangr206\'s blog</a>'
               . '<br> Judge program with c -- <a href="https://github.com/Hexcles/Eevee/blob/master/caretaker.c" target="_blank">Hexcles\'s Github</a>&nbsp;&nbsp;'
                        . '<a href="https://github.com/BYVoid/vakuum" target="_blank">BYVoid\' Github</a>'
          . '</div>'
     . '</li>';
echo '<li><font size="3em"><b>Have a nice day.</b></font></li>';
echo '<li><a href="https://github.com/Alrash/Judge-Online" target="_blank">' . $about['right']['license']['getCode'] . '</a></li>';