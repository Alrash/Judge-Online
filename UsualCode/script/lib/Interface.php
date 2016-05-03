<?php
/**
 * Created by PhpStorm.
 * User: lovelive
 * Date: 4/18/16
 * Time: 10:54 PM
 */
interface DataBase
{
    public function select($column, $table, $where = "", $extra = "");
    public function insert($table, $value, $column = "");
    public function update($table, $column, $where = "");
    public function delete($table, $where = "");
}
?>
