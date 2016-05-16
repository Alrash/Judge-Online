<?php
/**
 * Author: Alrash
 * 用于处理个人信息
 */
//暂时注释，会提示类已覆盖
require_once LIB_PATH . 'phpass/PasswordHash.php';
require_once LIB_PATH . 'String.php';

class InfoController extends Controller
{
    protected function checkNicknameFormat($nickname)
    {
        //这句防止出现\
        if (strpos($nickname, '\\') != 0 || $nickname[0] == '\\')
            return false;
        //这里表示不能取的值
        //这里有一个bug，\始终不能正确匹配
        return preg_match('/^[^<>?|\\/()*&\^%$#@!~`\'":;{}\[\]]{4,}$/', $nickname);
    }
    
    protected function checkEmailFormat($email)
    {
        return preg_match('/\w[-\w.+]*@([A-Za-z0-9][-A-Za-z0-9]+\.)+[A-Za-z]{2,14}/', $email);
    }

    function checkNickname()
    {
        $this->updateToken();
        $nickname = $_GET['nickname'];
        
        if ($this->checkNicknameFormat($nickname))
        {
            $info = new InfoModel;
            $this->set('result', $info->checkNickname($nickname));
        }
        else
            $this->set('result', 'fail');
    }

    function checkEmail()
    {
        $this->updateToken();
        $email = $_GET['email'];
        
        if ($this->checkEmailFormat($email))
        {
            $info = new InfoModel;
            $this->set('result', $info->checkEmail($email));
        }
        else
            $this->set('result', 'fail');
    }
    
    //实际没用的函数/方法，为了不暴露updateToken而设置的
    //使用ajax传参的时候，给出一个随机值n，但是这里不处理
    function randomNumber()
    {
        $this->updateToken();
    }
    
    //临时为java课程设计开的接口
    function getToken()
    {
        $this->updateToken();
        echo $_SESSION['token'];
    }
    
    //更新加密key
    final private function updateToken()
    {
        $_SESSION['token'] = md5(getSpecialRandString(6, 32));
    }
    
    //解密传输中的密码
    protected function decodePassword($passwd, $token)
    {
        //解密passwd(aes默认加密)
        //最后一个参数是$iv，至于这样写，是因为javascript默认加密时，iv只用了前16位
        //注意，LoginAndRegister.js内，未指明iv用16位，可能存在漏洞
        return mcrypt_decrypt(MCRYPT_RIJNDAEL_128, $token, base64_decode($passwd), MCRYPT_MODE_CBC, substr($token, 0, 16));
    }
    
    /**
     * 注册方法/函数
     */
    function addNewUser()
    {
        $email = $_POST['email'];
        $nickname = $_POST['nickname'];
        
        //去除发送时的多出的一个字符
        //why? 详见LoginAndRegister.js ('#submit').click部分
        $passwd = substr($_POST['passwd'], 0, strlen($_POST['passwd']) - 1);
        
        //声明加密哈希变量
        $hasher = new PasswordHash(8, false);
        //真正加密用户密码
        $passwdHash = $hasher->HashPassword($this->decodePassword($passwd, $_SESSION['token']));
        //构造insert使用的数组
        $columnvalue = array('Nickname' => $nickname, 'Passwd' => $passwdHash, 'Email' => $email);
        
        //执行插入,返回true表示插入成功
        $info = new InfoModel;
        $response = $info->addNewUser($columnvalue);
        
        if ($response)
        {
            //设置session,但是不设置大部分cookie
            unset($_SESSION['token']);

            //取出信息，存入session中
            $result = $info->getValues(array('*'), 'User_View', '`Nickname` = ' . addQuotes($nickname));
            foreach ($result as $key => $item)
                foreach ($item as $key => $value)
                    $_SESSION[$key] = $value;
            $_SESSION['signIn'] = true;

            $this->set('result', 'ok');
        }
        else
        {
            $this->updateToken();
            $this->set('result', 'fail');
        }
    }
    
    
    /**
     * 登录方法/函数
     */
    function signIn()
    {
        $name = $_POST['nickname'];
        $passwd = substr($_POST['passwd'], 0 ,strlen($_POST['passwd']) - 1);
        $remember = $_POST['remember'];
        $hasher = new PasswordHash(8, false);
        
        $info = new InfoModel;
        $response = $info->getValues(array('UId', 'Passwd'), 'UserInfo', '`Email` = ' . addQuotes($name) .
                ' or `Nickname` = ' . addQuotes($name));
        
        if (!is_null($response) && $response == 'Fail')
        {
            $this->set('result', "don't get");
        }
        else
        {
            if (is_null($response))
            {
                $this->set('result', 'fail');
            }
            else
            {
                //校验密码
                if ($hasher->CheckPassword($this->decodePassword($passwd, $_SESSION['token']), $response[0]['Passwd']))
                {
                    //设置cookie生存时间
                    if ($remember == 1)
                        $timeoffset = LIFTTIME;
                    else
                        $timeoffset = 3600;
                    
                    //设置session和cookie
                    setcookie(session_name(), session_id(), time() + $timeoffset, '/');
                    setcookie('LANG', '_zh_CN', time() + $timeoffset, '/');

                    //取出信息，存入session中
                    $response = $info->getValues(array('*'), 'User_View', '`UId` = ' . $response[0]['UId']);
                    foreach ($response as $key => $item)
                        foreach ($item as $key => $value)
                            $_SESSION[$key] = $value;
                    $_SESSION['signIn'] = true;

                    //记住登录状态的情况
                    if ($remember == 1)
                    {
                        setcookie(session_name(), session_id(), time() + $timeoffset, '/');
                        //获得用户名与密码
                        $response = $info->getValues(array('UId', 'Passwd'), 'UserInfo', '`UId` = ' . $_SESSION['UId']);
                        //记录用户名
                        setcookie('UID', $response[0]['UId'], time() + $timeoffset, '/');
                        //用于检验用户名
                        $crc = sprintf('%u', crc32($response[0]['UId']));
                        setcookie('UID_CHK', $crc, time() + $timeoffset, '/');
                        //用于重复登录
                        setcookie('DATA', sha1($response[0]['UId'] . sha1($response[0]['Passwd'], true)), time() + $timeoffset, '/');
                    }

                    $this->set('result', 'ok');
                }
                else
                {
                    $this->set('result', 'fail');
                }
            }
        }
    }

    function __destruct()
    {
        // TODO: Implement __destruct() method.
        $this->_view->showDealResult();
    }
}