<?php
 
class IndexController extends Controller
{
    function index() {
        $this->set('Title', 'index');
        $this->set('bodyStyle', 'index');
    }
}