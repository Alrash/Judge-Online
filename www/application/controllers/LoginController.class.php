<?php
/**
 * Author: Alrash
 * 用于用户的注册与登录
 */
class LoginController extends Controller
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
    
    function site($page = 'index')
    {
        $this->set('Title', 'info');
        $this->set('bodyStyle', ' ');
        $this->set('includePage', $page);
        $this->set($page, 'clicked');
        $this->set('arrayIndex', $page);
        $this->set('LOGO', false);
    }
}