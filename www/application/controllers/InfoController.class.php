<?php
/**
 * Author: Alrash
 * 用于处理个人信息
 */
//暂时注释，会提示类已覆盖
//require_once LIB_PATH . 'phpass/PasswordHash.php';
require_once LIB_PATH . 'String.php';

class InfoController extends Controller {
    protected function checkNicknameFormat($nickname) {
        //这句防止出现\
        if (strpos($nickname, '\\') != 0 || $nickname[0] == '\\')
            return false;
        //这里表示不能取的值
        //这里有一个bug，\始终不能正确匹配
        return preg_match('/^[^<>?|\\/()*&\^%$#@!~`\'":;{}\[\]]{4,}$/', $nickname);
    }
    
    protected function checkEmailFormat($email) {
        return preg_match('/\w[-\w.+]*@([A-Za-z0-9][-A-Za-z0-9]+\.)+[A-Za-z]{2,14}/', $email);
    }

    protected function checkNickname() {
        $this->updateToken();
        $nickname = $_GET['nickname'];
        
        if ($this->checkNicknameFormat($nickname)) {
            $info = new InfoModel;
            return $info->checkNickname($nickname);
        }
        else
            return 'fail';
    }

    protected function checkEmail() {
        $this->updateToken();
        $email = $_GET['email'];
        
        if ($this->checkEmailFormat($email)) {
            $info = new InfoModel;
            return $info->checkNickname($email);
        }
        else
            return 'fail';
    }
    
    //实际没用的函数/方法，为了不暴露updateToken而设置的
    //使用ajax传参的时候，给出一个随机值n，但是这里不处理
    protected function randomNumber() {
        $this->updateToken();
    }
    
    //更新加密key
    final private function updateToken() {
        $_SESSION['token'] = md5(getSpecialRandString(6, 32));
    }
    
    //解密传输中的密码
    protected function decodePassword($passwd, $token) {
        //解密passwd(aes默认加密)
        //最后一个参数是$iv，至于这样写，是因为javascript默认加密时，iv只用了前16位
        //注意，LoginAndRegister.js内，未指明iv用16位，可能存在漏洞
        return mcrypt_decrypt(MCRYPT_RIJNDAEL_128, $token, base64_decode($passwd), MCRYPT_MODE_CBC, substr($token, 0, 16));
    }
    
    /**
     * 注册方法/函数
     */
    protected function register() {
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
        
        if ($response) {
            //设置session,但是不设置大部分cookie
            unset($_SESSION['token']);

            //取出信息，存入session中
            $result = $info->getValues(array('*'), 'User_View', '`Nickname` = ' . addQuotes($nickname));
            foreach ($result as $key => $item)
                foreach ($item as $key => $value)
                    $_SESSION[$key] = $value;
            $_SESSION['signIn'] = true;

            return 'ok';
        }
        else {
            $this->updateToken();
            return 'fail';
        }
    }
    
    
    /**
     * 登录方法/函数
     */
    protected function signIn() {
        $name = $_POST['nickname'];
        $passwd = substr($_POST['passwd'], 0 ,strlen($_POST['passwd']) - 1);
        $remember = $_POST['remember'];
        $hasher = new PasswordHash(8, false);
        
        $info = new InfoModel;
        $response = $info->getValues(array('UId', 'Passwd'), 'UserInfo', '`Email` = ' . addQuotes($name) .
                ' or `Nickname` = ' . addQuotes($name));
        
        if (!is_null($response) && $response == 'Fail'){
            //$this->set('result', "don't get");
            return "don't get";
        }else {
            if (is_null($response)){
                return 'fail';
            }else {
                //校验密码
                if ($hasher->CheckPassword($this->decodePassword($passwd, $_SESSION['token']), $response[0]['Passwd'])){
                    //设置cookie生存时间
                    if ($remember == 1)
                        $timeoffset = LIFTTIME;
                    else
                        $timeoffset = 3600;
                    
                    //设置session和cookie
                    setcookie(session_name(), session_id(), time() + $timeoffset, '/');
                    setcookie('LANG', $_COOKIE['LANG'], time() + $timeoffset, '/');

                    //取出信息，存入session中
                    $response = $info->getValues(array('*'), 'User_View', '`UId` = ' . $response[0]['UId']);
                    foreach ($response as $key => $item)
                        foreach ($item as $key => $value)
                            $_SESSION[$key] = $value;
                    $_SESSION['signIn'] = true;
                    setcookie('SignIn', 1, time() + $timeoffset, '/');

                    //记住登录状态的情况
                    //额。。。以前写的，现在没有用，暂时保留 by 2016.08.13
                    if ($remember == 1){
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

                    return 'ok';
                }else {
                    $this->set('result', 'fail');
                    return 'fail';
                }
            }
        }
    }

    /* *
     * 查询数据库，返回json格式的数据
     */
    function getInfoWithJson() {
        $info = new InfoModel();
        switch($_GET['mode']) {
            case "user":
                return json_encode($info->getInfo(array('UId', 'Nickname', 'Email'), 'User_View', "UId != 1"));
            case "question":
                $column = array('PId' , 'Title');
                $type = "";
                if (isset($_GET['type']) && !is_null($_GET['type'])){
                    if ($_GET['type'] == '0' || $_GET['type'] == '1'){
                        $type = "Type like '%" . $_GET['type'] . "%' ";
                    }
                }
                $pid = "";
                if (isset($_GET['pid']) && !is_null($_GET['pid'])){
                    if (preg_match("/^\\d+$/", $_GET['pid']))
                        $pid = "PId = " . $_GET['pid'];
                }
                $title = "";
                if (isset($_GET['title']) && !is_null($_GET['title'])){
                    $tmp = preg_split("/ /", $_GET['title']);

                    if (sizeof($tmp)){
                        $title = "(Title like '%$tmp[0]%'";
                        for ($i = 1; $i < sizeof($tmp); $i++){
                            $title .= " or Title like '%$tmp[$i]%'";
                        }
                        $title .= ")";
                    }
                }
                $hardMode = "";
                if (isset($_GET['hard']) && !is_null($_GET['hard'])){
                    if (preg_match("/^(\\d+(;\\d+)*)|((\\d;)+)$/", $_GET['hard'])){
                        $tmp = preg_split("/;/", substr($_GET['hard'], 0, preg_match("/(\\d+;)+/", $_GET['hard']) ? (strlen($_GET['hard']) - 1) : strlen($_GET['hard'])));

                        if (sizeof($tmp)){
                            $hardMode = "(hard = $tmp[0]";
                            for ($i = 1; $i < sizeof($tmp); $i++){
                                $hardMode .= " or hard = $tmp[$i]";
                            }
                            $hardMode .= ")";
                        }
                    }
                }
                if (isset($_GET['info']) && $_GET['info'] == "long"){
                    if (isset($_COOKIE['lang']) && $_COOKIE['lang'] == "zh_CN"){
                        $typeName = 'Type_CN';
                    }else if (getSystemLanguage() == 'zh_CN'){
                        $typeName = 'Type_CN';
                    }else {
                        $typeName = 'Type_EN';
                    }
                    $typeName .= ' as Type';
                    $column = array('PId', 'Title', $typeName, 'Hard', 'Per', 'Total');
                }
                $page = 1;
                if (isset($_GET['page']) && preg_match("/\\d+/", $_GET['page']) && $_GET['page'] > 0){
                    $page = $_GET['page'];
                }
                $extra = "";
                if (isset($_GET['info']) && $_GET['info'] == 'long'){
                    $extra = "limit " . ($page - 1) * RECORD_SIZE  . ", " . RECORD_SIZE;
                    $data = $info->getInfo($column, "Question_View", $this->spliceWhere($type, $hardMode, $pid, $title), $extra);
                    $data = $data == null ? Array() : $data;
                    array_push($data, array("now" => $page, "total"=>floor($info->getRecordSize() / RECORD_SIZE + 1)));
                }else {
                    $data = $info->getInfo($column, "Question_View", $this->spliceWhere($type, $hardMode, $pid, $title), $extra);
                }
                return json_encode($data);
            default:
                return json_encode('');
        }
    }

    private function spliceWhere($type, $hardMode, $pid, $title){
        $where = ($type == "" ? "" : "$type and") . ($hardMode == "" ? "" : " $hardMode and");
        if ($title == "" && $pid == "") {
            $where = preg_split("/and$/", $where)[0];
        }else{
            $where .= " (" . $title . ($title != "" && $pid != "" ? " or " : "") . $pid . ")";
        }
        return trim(preg_replace("/ {2,}/", " ", $where));
    }

    /* *
     * 处理提交题目答案，并更新数据库（hardMode，个人信息Exp之类，Submission）
     * 更新规则:
     */
    public function submission(){
        //用于是否登录
        if (!isset($_SESSION['signIn']) || !$_SESSION['signIn']){
            return 'logout';
        }

        if (!isset($_GET['code']))
            return 'fail';

        $submit = new InfoModel();
        if (!isset($_POST['pid']) || is_null($submit->checkPid($_POST['pid']))){
            return 'fail';
        }

        $timestamp = date("Y-m-d H:i:s.u", time());
        if (!$submit->updateSubmission($_POST['pid'], $_SESSION['UId'], $_POST['compiler'], $timestamp))
            return 'fail';
        $sid = $submit->getSubmissionID($_POST['pid'], $_SESSION['UId'], $timestamp);
        //$sid = $submit->getSubmissionID(1, 2, "2016-08-16 00:00:01.000000");
        switch ($_POST['compiler']){
            case 'c':
                $extension = 'c';
                break;
            case 'c++':
            case 'c++11':
                $extension = 'cpp';
                break;
            case 'java':
                $extension = 'java';
                break;
            case 'python':
                $extension = 'py';
                break;
            default:
                return 'fail';
        }

        $path = DATA_PATH . "Submit/$sid/";
        mkdir($path, 0777, true);

        $type = $submit->getProblemType($_POST['pid']);
        if ($type == 0){
            //直接写入文件['code']
            if (($file = fopen($path . "$sid.$extension", "w"))){
                fwrite($file, $_POST['code']);
                fclose($file);
            }else {
                return 'permission';
            }
        }else if ($type == 1){
            //获取题目，拆分['code'] #@@#
            //填充写入
            $source = DATA_PATH . "Questions/" . $_POST['PId'] . "/source.$extension";
            if (file_exists($source) && ($file = fopen($source, "r"))){
                $answer = explode("#@@#", $_POST['code']);
                $pos = 0;
                if (!($dest = fopen($path . "$sid.$extension", "w")) || !($answerFile = fopen($path . "answer.txt", "w"))){
                    return 'permission';
                }

                /* *
                 * 填空题，写入answer.txt文件
                 */
                foreach ($answer as $key => $value){
                    fwrite($answerFile, $value . "\n");
                }
                fclose($answerFile);

                while (!feof($file)){
                    $lineStr = fgets($file);
                    $item = explode("____", $lineStr);
                    $line = $item[0];
                    for ($i = 1; $i < sizeof($item); $i++){
                        $line .= $answer[$pos] . $item[$i];
                        $pos++;
                        if ($pos == sizeof($answer))
                            return 'fail';
                    }
                    fwrite($dest, $line);
                }
                fclose($file);
                fclose($dest);
                if ($pos != sizeof($answer))
                    return 'fail';
            }else{
                return 'existed';
            }
        }

        $status = 0;
        //暂缺type
        /* *
         * 参数没有具体定下来，目前的顺序：
         *     sid 语言类型(type中确定) pid 文件数$submit->getProblemIni($_POST['pid'])['testFile']
         *     ['time']去s ['memory']去MB之类
         */
        exec(EXEC_PATH . "judge $sid " . $_POST['pid'] . " 3 4 5 6 7 9", $log, $status);
        if ($status != 0){
            return "judge";
        }

        return 'ok';
    }

    public function getRecentSubmission(){
        if (isset($_GET['pid']) && is_numeric($_GET['pid'])){
            $info = (new InfoModel())->getRecentSubmission($_GET['pid']);
            return json_encode($this->modifyRecentSubmission($info));
        }
        return json_encode("");
    }

    private function modifyRecentSubmission($info){
        if (is_null($info))
            return "";

        foreach ($info as $key => $item){
            $item['SId'] = $this->modifySId($item['SId']);
            $item['time'] = $this->modifyTime($item['time']);
            $info[$key] = $item;
        }
        return $info;
    }

    private function modifySId($sid){
        return "<a href='#$sid' target='_blank'>$sid</a>";
    }

    /* *
     * 修正时间 -- 0.01s 1h4min之类
     */
    private function modifyTime($time){
        if ($time < 60){
            return '-' . number_format($time, 2) . 's';
        }else if ($time < 3600){
            return '-' . floor($time / 60) . 'min' . floor($time) % 60 . 's';
        }else if ($time < 19800){
            return '-' . floor($time / 3600) . 'h' . floor($time % 3600 / 60) . 'min';
        }else{
            return 'infinity';
        }
    }

    /**
     * 信息处理模块的唯一接口
     * 使用nginx转向时，website/info(这是接口，而非控制器)/实际动作名
     * */
    public function info($realAction = null) {
        $result = null;
        if (!method_exists($this, $realAction))
            $result = 'fail';
        else {
            $result = $this->$realAction();
        }
        
        $this->set('result', $result);
    }

    function __destruct() {
        // TODO: Implement __destruct() method.
        $this->_view->showDealResult();
    }
}