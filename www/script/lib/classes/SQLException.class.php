<?php

/**
 * Author : Alrash
 * 这是一个数据库异常抛出类
 */
class SQLException extends Exception
{
    private $errMesg;
    private $errNo;
    
    public function __construct($message = "Unknown Problem", $no = 1)
    {
        $this->errMesg = $message;
        $this->errNo = $no;
    }

    public function getErrMesg()
    {
        return $this->errMesg;
    }
    
    public function getErrNo()
    {
        return $this->errNo;
    }
}