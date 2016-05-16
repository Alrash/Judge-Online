<?php

// 初始化常量
defined('ROOT') or define('ROOT', __DIR__.'/');
defined('APP_PATH') or define('APP_PATH', dirname($_SERVER['SCRIPT_FILENAME']).'/');
defined('APP_DEBUG') or define('APP_DEBUG', false);
defined('DATA_PATH') or define('DATA_PATH', APP_PATH .'DATA/');
defined('STATIC_PATH') or define('STATIC_PATH', APP_PATH . 'public/');
defined('SCRIPT_PATH') or define('SCRIPT_PATH', APP_PATH . 'script/');
defined('LIB_PATH') or define('LIB_PATH', SCRIPT_PATH . 'lib/');
defined('CONFIG_PATH') or define('CONFIG_PATH', APP_PATH.'config/');
defined('RUNTIME_PATH') or define('RUNTIME_PATH', APP_PATH.'runtime/');

const EXT = '.class.php';

// 包含配置文件
require APP_PATH . 'config/config.php';

//包含核心框架类
require ROOT . 'Core.php';

// 实例化核心类
$fast = new Fast;
$fast->run();