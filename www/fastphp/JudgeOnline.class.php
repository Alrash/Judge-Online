<?php
/**
 * Author:Alrash
 * time: 2016.05.16
 * 本class用于实例化数据库接口(DataBase.interface.php)
 * 本class使用mysql数据库，使用mysqli扩展，但未直接继承mysqli
 * 开放的方法：
 *      除selectArray selectArraySpecialIndex selectForAnotherRequest insert update delete外，
 *      另包含几个sql拼接语句的方法，均以create开头
 * 各方法作用具体参见具体定义
 * 各方法返回值：
 *      selectArray selectArraySpecialIndex 返回数组，具体见定义
 *      insert update delete 成功返回true，失败返回false
 *      createXXX返回拼接好的sql语句
 *      selectForAnotherRequest未实现
 * 本类未对参数进行检测，请谨慎检查实參，然后传值
 * 特别说明：
 *  (1) 本类不能使用聚合函数，若希望使用，请实现selectForAnotherRequest或增加方法或派生子类...
 *  (2) 调用没一个可执行sql语句的函数时，均已先connect数据库，方法调用最后关闭
 * 
 * 为什么不用预处理和数组传参?：
 *  (1) 懒              (╯‵□′)╯︵┻━┻
 *  (2) 保证通用性       （つ＞ω●）つ
 *  (3) 还没学到没那么深  -_-|||
 * 另注，偷懒复制粘贴了几行，勿见怪(ゝ∀･)
 * 若需要知道更改/影响了几行，可选择加入public function getRow()和 private function setRow($var)
 *      例：$this->mysql->query($sql); ...
 *          可改成$this->mysql->query($sql);
 *               $this->setRow($this->mysql->affected_rows); ...
 */ 
require_once(LIB_PATH . 'Interface/DataBase.interface.php');
require_once(LIB_PATH . 'String.php');
require_once(LIB_PATH . 'classes/SQLException.class.php');
require_once(LIB_PATH . 'classes/Log.class.php');

class JudgeOnline implements DataBase
{
    /*数据库的组成五要素*/
    protected $databasename;
    protected $host;
    protected $port;
    protected $username;
    protected $passwd;

    //mysql数据库实例
    private $mysql;

    function __construct($name, $user, $passwd, $host = "localhost", $port = "3306")
    {
        $this->databasename = $name;
        $this->username = $user;
        $this->passwd = $passwd;
        $this->host = $host;
        $this->port = $port;
    }

    //连接数据库，若连接异常，则抛出异常
    protected function connect()
    {
        /*$log = new Log();
        $log->setMessage("be ready to connect " . $this->databasename . " used by " . $this->username);
        $log->outToLog();*/
        $this->mysql = new mysqli($this->host, $this->username, $this->passwd, $this->databasename);

        if (mysqli_connect_errno())
            throw new SQLException(mysqli_connect_error(), mysqli_connect_errno());
    }

    //关闭数据库连接，释放占用资源
    protected function closemysql()
    {
        $this->mysql->close();
    }

    /**
     * 查询操作
     * 若执行错误，返回"Fail"
     * 若查询结果为空，返回null
     * 否则，返回二维数组，0～n - 1为行下标，列名为列下标
     * */
    public function selectArray($sql)
    {
        // TODO: Implement selectArray() method.
        try
        {
            $this->connect();
            $result = $this->mysql->query($sql);
            $count = $result->num_rows;
            if ($count == 0)                        //查询结果为空，返回null
                return null;

            $i = 0;
            $arr = array();
            $rowname = array();

            while ($field = $result->fetch_field())
            {//获得列名
                $rowname[$i] = $field->name;
                $i++;
            }
            $i = 0;
            while ($row = $result->fetch_array(MYSQLI_BOTH))
            {//获得字段值，并追加至arr
                $tmp = array();
                foreach ($rowname as $value)
                    $tmp[$value] = $row[$value];
                $arr[$i] = $tmp;
                $i++;
            }

            $result->free();
            $this->closemysql();
        }
        catch (mysqli_sql_exception $e)
        {
            echo $e->getMessage();
            return "Fail";
        }
        catch (SQLException $e)
        {
            echo $e->getErrMesg();
            $this->closemysql();
            return "Fail";
        }

        return $arr;
    }

    /**
     * 此函数是用于返回特殊下标(键值)的select方法
     * select函数需注意的地方，该函数也会注意
     * 注意：因本函数使用indexKey参数指向的键所代表的值进行索引，
     *      请保证这个键所对应的值唯一
     *  例：
     *      selectArraySpecialIndex($sql, 'UId')    代表使用UId进行索引，
     *                                                  返回值为array('2' => array(...), '4' => array(...) ...)
     *      selectArratSpecialIndex($sql, 'Email')  返回为array('kasukuikawai@gmail.com' => array(...), 
     *                                                              '1607768311@qq.com' => array(...) ...)
     */
    public function selectArraySpecialIndex($sql, $indexKey)
    {
        // TODO: Implement selectArraySpecialIndex() method.
        try
        {
            $this->connect();
            $result = $this->mysql->query($sql);
            $count = $result->num_rows;
            if ($count == 0)                        //查询结果为空，返回null
                return null;

            $i = 0;
            $arr = array();
            $rowname = array();

            while ($field = $result->fetch_field())
            {//获得列名
                $rowname[$i] = $field->name;
                $i++;
            }
            while ($row = $result->fetch_array(MYSQLI_BOTH))
            {//获得字段值，并追加至arr
                $tmp = array();
                foreach ($rowname as $value)
                    $tmp[$value] = $row[$value];
                $arr[$tmp[$indexKey]] = $tmp;
            }

            $result->free();
            $this->closemysql();
        }
        catch (mysqli_sql_exception $e)
        {
            echo $e->getMessage();
            return "Fail";
        }
        catch (SQLException $e)
        {
            echo $e->getErrMesg();
            $this->closemysql();
            return "Fail";
        }

        return $arr;
    }
    
    //没有实现，全部返回Fail
    public function selectForAnotherRequest($sql)
    {
        // TODO: Implement selectForAnotherRequest() method.
        return 'Fail';
    }

    /**
     * 执行sql语句
     * 执行正确返回true, 错误返回false
     * */
    final private function exec($sql)
    {
        try
        {
            //连接数据库
            $this->connect();

            //执行sql语句
            $this->mysql->query($sql);
            $this->closemysql();
        }
        catch (mysqli_sql_exception $e)
        {
            //数据库执行错误
            echo $e->getMessage();
            $this->closemysql();
            return false;
        }
        catch (SQLException $e)
        {
            //连接数据库错误
            echo $e->getErrMesg();
            return false;
        }

        return true;
    }

    /**
     * 插入操作
     */
    public function insert($sql)
    {
        // TODO: Implement insert() method.
        return $this->exec($sql);
    }

    //更新操作
    public function update($sql)
    {
        // TODO: Implement update() method.
        return $this->exec($sql);
    }

    //删除操作
    public function delete($sql)
    {
        // TODO: Implement delete() method.
        return $this->exec($sql);
    }

    /**
     * 拼接获得select语句字符串
     * 参数一为数组，包含所需查找的列名
     * */
    public function createSelectSql($columnarray = array(), $table, $where = '', $extra = '')
    {
        $column = '';
        foreach ($columnarray as $key => $item)
            $column .= $item . ', ';
        $column = substr_replace($column, '', strlen($column) - 2);

        //拼接sql语言字符串
        $sql = 'select ' . $column . ' from ' . $table;
        if ($where != '')
            $sql = $sql . ' where ' . $where;

        return  $sql . ' ' . $extra;
    }

    /**
     * 参数2为数组（键值对的形式），实际必须为2维组
     * 例：对UserInfo表操作
     *      createInsertSql('UserInfo', array([0]=>array("UId"=>1, "Nickname"=>"alrash", "email"=>"..."),
     *                               [1]=>array("UId"=>2, "Nickname"=>"bwfullcolr", "email"=>"...")));
     *     表示可以插入两行值，sql为insert UserInfo(UId, Nickname, email) values(...),(...)
     * 当为一维数组时，会被自动变为2维组，索引为0
     * */
    public function createInsertSql($table, $columnvalue = array())
    {
        //若参数2为一维？数组，转换成array(key=>array())的形式
        if (!is_array($columnvalue[key($columnvalue)]))
            $columnvalue = array(0 => $columnvalue);

        //将键值组装成(`列名`，`列名`，...)的形式
        //第三行是将末尾的', '转换成)
        $columnname = "(";
        foreach ($columnvalue[0] as $key => $item)
            $columnname .= "`" . $key . "`, ";
        $columnname = substr_replace($columnname, ")", strlen($columnname) - 2);

        //将值组装成("值", "值", ...),("值", "值", ...)...的形式
        $values = "";
        foreach ($columnvalue as $key => $item)
        {
            $values .= "(";
            foreach ($item as $key => $items)
                $values .= addQuotes($items) . ", ";
            $values = substr_replace($values, "), ", strlen($values) - 2);
        }
        $values = substr_replace($values, "", strlen($values) - 2);

        //拼接sql语句
        $sql = "insert into " . $table . $columnname . " values " . $values;

        return $sql;
    }

    public function createUpdateSql($table, $column, $where = "")
    {
        //拼接字符串
        $sql = "update " . $table . " set " . $column;
        if ($where != "")
            $sql = $sql . " where " . $where;
        return $sql;
    }

    public function createDeleteSql($table, $where = '')
    {
        //拼接字符串
        $sql = 'delete from ' . $table;
        if ($where != '')
            $sql .= ' where ' . $where;
        return $sql;
    }
}