<?php
/**
 * Author: Alrash
 * 介绍：数据库接口文件，规定数据库一般操作接口
 * 具体规范请见DataBase.class.php文件
 */
interface DataBase
{
    public function select($columnarray = array(), $table, $where = "", $extra = "");
    public function insert($table, $columnvalue = array());
    public function update($table, $column, $where = "");
    public function delete($table, $where = "");
}
