<?php
/**
 * Author: Alrash
 * 用于获取题目信息，以及从文件读取相关信息(少视图)
 * 关于题目信息的存放：为了减少对数据库的存取，原本计划将其读入内存一段时间，但暂时未实现
 *                  从文件读取具体题目时，文件内本应仅存放题目具体内容，但是现在多存放题目名等冗余信息
 * due to fcitx issue, I can not use Chinese with keyboard
 */

class QuestionModel extends Model{
    function getDetailInformation($pid){
        $path = DATA_PATH . 'Questions/' . $pid;
        $detail = $path . '/detail.ini';
        $content_file = $path . '/content.txt';

        if (file_exists($content_file) && file_exists($detail) && ($file = fopen($content_file, 'r'))){
            $content = null;
            while(!feof($file)){
                //大前提：上传时，已改装完毕╮(￣▽￣)╭
                $content .= $this->removeLineBreak(fgets($file));
            }
            fclose($file);

            $ini = parse_ini_file($detail);
            $ini['content'] = $content;

            return $ini;
        }else{
            //文件不存在
            return null;
        }
    }

    function getCompilerChoice(){
        $path = DATA_PATH . "compiler.xml";
        return simplexml_load_file($path);
    }

    function removeLineBreak($line){
        return str_replace("\n", "", str_replace("\r", "", $line));
    }

    function getHardMode($pid){
        return $this->selectArray("select Hard from `QuestionInfo` where Pid = $pid");
    }

    function getQuestion($page){
        $extra = "limit " . ($page - 1) * RECORD_SIZE . ", " . RECORD_SIZE;
        $type = "Type_CN";
        if (isset($_COOKIE['LANG']) && $_COOKIE['LANG'] != 'zh_CN'){
            if (getSystemLanguage() != "zh_CN")
                $type = "Type_EN";
        }
        return $this->selectArray("select $type as Type, Title, Pid, Hard, Total, Per from Question_View $extra");
    }

    /* *
     * 获取总题目数
     */
    public function getRecordSize(){
        return $this->selectArray("select count(PId) as size from Question_View")[0]['size'];
    }

    public function getQuestionTop20($pid){
        return $this->selectArray($this->createSelectSql(array("Nickname", "Runtime", "Runmemory", "compiler", "SId"),
            "Submission_View", "PId = $pid", "Order BY  `Runtime` ASC, `Runmemory` ASC limit 0, 20"));
    }

    public function getRecentSubmission($pid){
        return $this->selectArray("select `Nickname`, `SId`, `compiler`, `Status`,
            (UNIX_TIMESTAMP() - UNIX_TIMESTAMP(`timestamp`)) as `time` 
            from Submission_View where PId = $pid 
              and (UNIX_TIMESTAMP(`timestamp`) > (UNIX_TIMESTAMP() - 3600 * 6) and UNIX_TIMESTAMP(`timestamp`) < UNIX_TIMESTAMP())
               order by `timestamp` DESC limit 0, 10");
    }
}