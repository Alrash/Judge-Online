<?php

/**
 * Created by PhpStorm.
 * User: lovelive
 * Date: 4/25/16
 * Time: 5:09 PM
 */
require_once(dirname(dirname(dirname(__FILE__))) . '/config/config.php');

class Log
{
    private $err;
    private $time;
    private $message;
    
    public function __construct()
    {
        $this->time = "[" . date("Y-m-d H:i:s") . "]";
        $this->err = "";
        $this->message = "";
    }

    public function setErrorMessage($err)
    {
        $this->err = $err;
    }

    public function outToErrorLog()
    {
        $log = $this->time . " Error: " . $this->err;
        error_log($log, 3, ERROR_LOG_ROOT . "/error.log");
    }

    public function setMessage($mesg)
    {
        $this->message = $this->err = $mesg;
    }

    public function outToLog()
    {
        $log = $this->time . " : " . $this->message;
        error_log($log, 3, EXEC_LOG_ROOT . "/exec.log");
    }
}