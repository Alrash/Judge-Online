<?php
/**
 * Author: Alrash
 * 用于用户的注册与登录
 */
class LogController extends Controller
{
    function signIn()
    {
        $this->set('Title', 'login');
        $this->set('bodyStyle', ' ');
    }
    
    function register()
    {
        $this->set('Title', 'reg');
        $this->set('bodyStyle', ' ');
    }
}