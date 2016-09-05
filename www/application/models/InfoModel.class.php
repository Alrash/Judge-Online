<?php
/**
 * 个人信息模型
 */

class InfoModel extends Model {
    function checkNickname($nickname) {
        $response = $this->selectArray($this->createSelectSql(array("Nickname"), "UserInfo",  "`Nickname` = " . addQuotes($nickname)));
        return $this->dealCheckResponse($response);
    }
    
    function checkEmail($email) {
        $response = $this->selectArray($this->createSelectSql(array("Email"), "UserInfo",  "`Email` = " . addQuotes($email)));
        return $this->dealCheckResponse($response);
    }
    
    /**
     * 用于处理check函数从模型获得的数据
     * 返回结果:don't get     数据库查询错误(Fail)
     *         ok            没有查询的值(null)，说明未被使用/注册
     *         fail          查询到值(有值),说明已被使用
     */
    final private function dealCheckResponse($response) {
        if (!is_null($response) && $response == "Fail")
            return "don't get";
        else
            if (is_null($response))
                return "ok";
            else
                return "fail";
    }
    
    function addNewUser($columnvalue = array()) {
        return $this->insert($this->createInsertSql('UserInfo', $columnvalue));
    }
    
    function getValues($column = array(), $table, $where) {
        return $this->selectArray($this->createSelectsql($column, $table, $where));
    }

    /**
     * @return ok
     * @return fail
     */
    public function checkPid($pid){
        if(is_null($this->selectArray("select `Pid` from QuestionInfo where `Pid` = $pid"))){
            return 'fail';
        }else
            return 'ok';
    }

    public function updateSubmission($pid, $uid, $compiler, $timestamp){
        return $this->insert($this->createInsertSql('Submission',
            array('Pid' => $pid, 'Uid' => $uid, 'compiler' => $compiler, 'timestamp' => "$timestamp")));
    }

    public function getSubmissionID($pid, $uid, $timestamp){
        return $this->selectArray("select SId from Submission_View where PId = $pid and UId = $uid and timestamp = '$timestamp'")[0]['SId'];
    }

    /* *
     * question type: 大题、填空题
     * @return 0 or 1
     *     0 大题
     *     1 填空题
     */
    public function getProblemType($pid){
        return $this->selectArray("select `Type` from QuestionInfo where PId = $pid")[0]['Type'];
    }

    /* *
     * 获得数组形式的配置文件信息
     */
    public function getProblemIni($pid){
        $path = DATA_PATH . "Questions/$pid/detail.ini";
        if (file_exists($path)){
            return parse_ini_file($path);
        }else{
            return null;
        }
    }

    /* *
     * 获取部分数据信息
     */
    public function getInfo($column = array(), $table, $where = '', $extra){
        return $this->selectArray($this->createSelectSql($column, $table, $where, $extra));
    }

    /* *
     * 获取总题目数
     */
    public function getRecordSize(){
        return $this->selectArray("select count(PId) as size from Question_View")[0]['size'];
    }

    public function getRecentSubmission($pid){
        return (new QuestionModel())->getRecentSubmission($pid);
    }
}