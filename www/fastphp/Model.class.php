<?php
class Model extends JudgeOnline {
    function __construct() {
 
        // 连接数据库
        parent::__construct(DATABASE, USER, PASSWD, HOST, PORT);
    }
 
    function __destruct() {
    }
}