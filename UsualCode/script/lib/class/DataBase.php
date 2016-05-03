<?php
/**
 * Author:Alrash
 * 本class用于实例化数据库接口
 * 本class使用mysql数据库，使用mysqli扩展，但未直接继承mysqli
 * 开放4个方法使用
 * select insert update delete
 * 各方法参数如同字面意，几乎是按sql语言的顺序进行设置（insert除外）
 * 各方法返回值：
 * select 返回数组，具体见定义
 * insert update delete 成功返回true，失败返回false
 * 本类未对参数进行检测，请谨慎检查实參，然后传值
 * 特别说明，虽然使用interface定义的接口，但是本类insert的第三个参数实际无用
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
require_once(dirname(dirname(__FILE__)) . '/Interface.php');
require_once(dirname(dirname(__FILE__)) . '/String.php');
require_once('SQLException.php');
require_once('Log.php');

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

    //执行sql语句
    //执行正确返回true, 错误返回false
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
     * 查询操作
     * 若执行错误，返回"Fail"
     * 若查询结果为空，返回null
     * 否则，返回二维数组，0～n - 1为行下标，列名为列下标
     */
    public function select($columnarray, $table, $where = "", $extra = "")
    {
        // TODO: Implement select() method.
        
        if (!is_array($columnarray))
        {
            echo "please use array at columnarray in select function.";
            return "Fail";
        }
        
        try
        {
            $this->connect();
            
            $column = "";
            foreach ($columnarray as $key => $item)
                $column .= $item . ", ";
            $column = substr_replace($column, "", strlen($column) - 2);
            
            //拼接sql语言字符串
            $sql = "select " . $column . " from " . $table;
            if ($where != "")
                $sql = $sql . " where " . $where;
            $sql = $sql . " " . $extra;
            
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
     * 插入操作
     * 参数2为数组（键值对的形式），必须为2维组
     * 例：对UserInfo表操作
     *      insert('UserInfo', array([0]=>array("UId"=>1, "Nickname"=>"alrash", "email"=>"..."),
     *                               [1]=>array("UId"=>2, "Nickname"=>"bwfullcolr", "email"=>"...")));
     *     表示可以插入两行值
     */
    public function insert($table, $columnvalue=array(), $none="")
    {
        // TODO: Implement insert() method.
        //检查参数二是否为数组形式
        if (!is_array($columnvalue)) 
        {
            echo "please use array in insert";
            return false;
        }

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
        
        return $this->exec($sql);
    }

    //更新操作
    public function update($table, $column, $where = "")
    {
        // TODO: Implement update() method.
        //拼接字符串
        $sql = "update " . $table . " set " . $column;
        if ($where != "")
            $sql = $sql . " where " . $where;
        
        return $this->exec($sql);
    }

    //删除操作
    public function delete($table, $where = "")
    {
        // TODO: Implement delete() method.
        //拼接字符串
        $sql = "delete from " . $table;
        if ($where != "")
            $sql = $sql . " where " . $where;

        return $this->exec($sql);
    }
}
?>
