<?php
/**
 * Auther: Alrash
 * 本模块？用于接受信息修改的异步js消息
 * 包括get和post方法
 */
//session_id($_COOKIE['PHPSESSID']);
if (!isset($_SESSION))
    session_start();

//定义生存时间
defined("LIFTTIME") or define("LIFTTIME", 3600 * 24 * 365);

/**
 * 从上至下：
 * 数据库配置文件
 * 封装的数据库类
 * 字符串处理文件
 * 第三方加密库phpass(Portable PHP password hashing framework)
 */
require_once('../config/config.php');
require_once('../lib/class/DataBase.php');
require_once('../lib/String.php');
require_once('../lib/phpass/PasswordHash.php');

//获得nickname的值
if (isset($_GET['nickname']))
    $nickname = $_GET['nickname'];
if (isset($_POST['nickname']))
    $nickname = $_POST['nickname'];
//获得email的值
if (isset($_GET['email']))
    $email = $_GET['email'];
if (isset($_POST['email']))
    $email = $_POST['email'];
//获得是否保持登录信息
//否：0 是：1
if (isset($_POST['remember']))
    $remember = $_POST['remember'];
/**
 * 获得passwd的值
 * 勿忘去除末尾的字符
 * 为什么加字符，请见LoginAndRegister.js  $("#submit").click
 */
if (isset($_POST['passwd']))
    $passwd = substr($_POST['passwd'], 0, strlen($_POST['passwd']) - 1);

function checkExist($column, $table, $where, $database, $user, $password, $host, $port)
{
    $mysql = new JudgeOnline($database, $user, $password, $host, $port);
    $result = $mysql->select($column, $table, $where);
    if (!is_null($result) && $result == "Fail")
        echo "don't get";
    else
        if (is_null($result))
            echo "ok";
        else
            echo "fail";
}

/**
 * 提交方式为Ajax，可以进行检测
 * 防止直接访问
 */
if(isset($_SERVER['HTTP_X_REQUESTED_WITH']) && strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) == 'xmlhttprequest')
{
    //检测是否为GET，因为POST太长，未检测
    if ($_SERVER['REQUEST_METHOD'] == 'GET')
    {
        //检测昵称
        if (isset($_GET['nickname']))
            checkExist(array('Nickname'), 'UserInfo', "`Nickname` = " . addQuotes($nickname),
                DATABASE, USER, PASSWD, HOST, PORT);
        //检测邮箱
        if (isset($_GET['email']))
            checkExist(array('Email'), 'UserInfo', "`Email` = " . addQuotes($email),
                DATABASE, USER, PASSWD, HOST, PORT);
        //每次get提交后，重新为token赋值，用于密码加密传输
        $_SESSION['token'] = md5(getSpecialRandString(6, 32));
    }
    else
    {
        //解密passwd
        //最后一个参数是$iv，至于这样写，是因为javascript默认加密时，iv只用了前16位
        //注意，LoginAndRegister.js内，未指明iv用16位，可能存在漏洞
        $decode = mcrypt_decrypt(MCRYPT_RIJNDAEL_128, $_SESSION['token'], base64_decode($passwd), MCRYPT_MODE_CBC, substr($_SESSION['token'], 0, 16));

        $hasher = new PasswordHash(8, false);
        
        /**
         * $_POST['email']是用于区分是登录还是注册
         * 登录使用$_POST['nickname'] 与 $_POST['passwd']
         * 注册使用多使用$_POST['email']
         */
        if (isset($_POST['email']))
        {
            //加密原passwd
            $passwdHash = $hasher->HashPassword($decode);
            
            //向数据库插入数据
            $mysql = new JudgeOnline(DATABASE, USER, PASSWD, HOST, PORT);
            $columnvalue = array('Nickname' => $nickname, 'Passwd' => $passwdHash, 'Email' => $email);
            if ($mysql->insert('UserInfo', $columnvalue))
            {
                //设置session,但是不设置大部分cookie
                unset($_SESSION['token']);

                //取出信息，存入session中
                $result = $mysql->select(array('*'), 'User_View', '`Nickname` = ' . addQuotes($nickname));
                foreach ($result as $key => $item)
                    foreach ($item as $key => $value)
                        $_SESSION[$key] = $value;
                $_SESSION['Signin'] = true;
                
                setcookie(session_name(), session_id(), time() + LIFTTIME, '/');
                
                echo "ok";
            }
            else
            {
                $_SESSION['token'] = md5(getSpecialRandString(16, 32));
                echo "fail";
            }
        }
        else
        {//登录情况
            $mysql = new JudgeOnline(DATABASE, USER, PASSWD, HOST, PORT);
            $result = $mysql->select(array('UId', 'Passwd'), 'UserInfo', '`Email` = ' . addQuotes($nickname) .
                ' or `Nickname` = ' . addQuotes($nickname));
            if (!is_null($result) && $nickname == "Fail")
                echo "don't get";
            else
            {
                if (is_null($result))
                    echo "fail";
                else
                {
                    if ($hasher->CheckPassword($decode, $result[0]['Passwd']))
                    {
                        //设置session和cookie
                        setcookie(session_name(), session_id(), time() + LIFTTIME, '/');

                        //取出信息，存入session中
                        $result = $mysql->select(array('*'), 'User_View', '`UId` = ' . $result[0]['UId']);
                        foreach ($result as $key => $item)
                            foreach ($item as $key => $value)
                                $_SESSION[$key] = $value;
                        $_SESSION['Signin'] = true;
                        
                        //记住登录状态的情况
                        if ($remember == 1)
                        {    
                            //获得用户名与密码
                            $result = $mysql->select(array('UId', 'Passwd'), 'UserInfo', '`UId` = ' . $_SESSION['UId']);
                            //记录用户名
                            setcookie("UID", $result[0]['UId'], time() + LIFTTIME, '/');
                            //用于检验用户名
                            $crc = sprintf("%u", crc32($result[0]['UId']));
                            setcookie("UID_CHK", $crc, time() + LIFTTIME, '/');
                            //用于重复登录
                            setcookie("DATA", sha1($result[0]['UId'] . sha1($result[0]['Passwd'], true)), time() + LIFTTIME, '/');
                            setcookie("Login", true, time() + LIFTTIME, '/');
                        }

                        echo "ok";
                    }
                    else
                        echo "fail";
                }
            }
        }
    }
}
?>