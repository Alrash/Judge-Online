<?php
/**
 * Created by PhpStorm.
 * User: lovelive
 * Date: 4/18/16
 * Time: 10:51 PM
 */
//数据库配置
defined('DATABASE') or define('DATABASE', 'JudgeOnline');
defined('HOST') or define('HOST', '127.0.0.1');
defined('PORT') or define('PORT', '3306');
defined('USER') or define('USER', 'JudgeOnline');
defined('PASSWD') or define('PASSWD', 'judgement');

//存储错误日志的根路径和执行日志路径
defined('ERROR_LOG_ROOT') or define('ERROR_LOG_ROOT', '/www/PhpStorm/Test');
defined('EXEC_LOG_ROOT') or define('EXEC_LOG_ROOT', '/www/PhpStorm/Test');

//judge执行路径，注意权限问题
defined('EXEC_PATH') or define('EXEC_PATH', '/home/alrash/Desktop/judge/judge/');

//每页显示大小
defined('RECORD_SIZE') or define('RECORD_SIZE', 30);
