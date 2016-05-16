<?php
/**
 * Author: Alrash
 * 介绍：数据库接口文件，规定数据库一般操作接口
 * 规范CRUD参数
 * */
interface DataBase
{
    /**
     * 三种select语句：
     * selectArray为返回一个二维表，每列为查询的列名所对应的值，行以1-n标记
     * selectArraySpecialIndex几乎同第一个，但是行以$indexKey所指定
     * selectForAnotherRequest处理含有聚合函数的sql语句
     * */
    public function selectArray($sql);
    public function selectArraySpecialIndex($sql, $indexKey);
    public function selectForAnotherRequest($sql);

    /**
     * 普通的增删改操作，成功返回true，失败返回false
     * */
    public function insert($sql);
    public function update($sql);
    public function delete($sql);
}
