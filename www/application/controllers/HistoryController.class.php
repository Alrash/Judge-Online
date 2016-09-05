<?php
/* *
 * Author: Alrash
 * Date: 2016/08/29 16:25
 * Description:
 */
class HistoryController extends Controller{
    public function __construct($controller, $action){
        parent::__construct($controller, $action);
        $this->set('LOGO', false);
        $this->set('MENU', false);
        $this->set('bodyStyle', ' ');
    }

    /* *
     * By 2016/08/30 10:30
     * 当前整理：（时间间隔太长，以致忘记部分提交、上传题目思路）
     *    1. 可上传的题目类型有两种： 填空题与综合大题
     *    2. 上传时，这两种存放的区别与联系：
     *       (1) 数据库QuestionInfo表Type字段记录，0代表综合大题，1代表填空题 | 记得该字段类型int --> tinyint
     *       (2) (补(2),建议先看(3))存放文件：
     *           同样 -- DATA/Question/pid/{content.txt, detail.ini, in/..., out/...}
     *           异 -- 填空题多存放source.$extension文件
     *           各文件(夹)意义: content.txt 题目题干 detail.ini 描述题目文件 in/ 输入样例
     *                          out/ 输出样例 source.$extension 填空题待替换文件
     *       (3) 其余均相同（这里有一点：上传时，没有阐明填空题语言类型，提交时，含有选择语言提交；
     *               这里作出部分当时思路的猜想
     *               a. 可以同时上传多个语言版本的题目，提交时，可从中选择一个；
     *                  但是，这里造成了提交的混乱 -- 提供所有的语言选项，但是仅显示一个语言的题目 -- 不知道其余题目,
     *                  并且在显示题目信息时，showDetail(QuestionController中)仅读取content.txt文件作为原内容输出，
     *                  而不是InfoController/submission使用的source.$extension
     *               b. 写的时候比较偷懒，没有检测填空题语言类型，需要依赖用户选择）
     *    3. 提交时，
     *       同时传入源码答案，选择的语言类型，pid，使用session中的uid，更新数据库获取sid；
     *       写入文件夹位置DATA/Submit/sid
     *       大题，直接取code，写入sid.$extension；填空题，code是由input1#@@#input2#@@#input3拼接而成，
     *           这里拆分，写入answer.txt，然后将拆分的内容，替换Question/pid/source.$extension中的位置，另存为sid.$extension文件
     * 总结：这里需要显示的部分
     *     综合题：sid/sid.$extension
     *     填空题：sid/answer.txt pid/source.$extension
     *
     * 本函数方法：搜寻数据库，获取compiler，pid，goal；
     *            分项得分(？ -- judged写入sid/goal.ini 不写数据库的原因；数据组不定，虽然可以写成一条varchar，然后分割)
     */
    public function submission($sid){
        $info = new HistoryModel();
        $this->set("Title", 'submission');

        $ini = $info->getQuestionInfo($sid);
        if (is_null($ini)){
            $this->set('existed', false);
            return;
        }
        $content = $info->getSubmissionContent($sid, $ini['pid'], $ini['type'], $ini['compiler']);
        if (is_null($content)){
            $this->set('existed', false);
            return;
        }
        $this->set('ini', $ini);
        $this->set('existed', true);
        $this->set('content', $content);
    }

    public function user(){
        $this->set('Title', 'user');
        //使用$_SESSION['uid']
        if(!isset($_SESSION['signIn']) || $_SESSION['signIn'] == false)
            return;

        $info = new HistoryModel();
        $pageSize = $info->getSubmissionNumber($_SESSION['UId']);
        $page = isset($_GET['page']) && preg_match("/^[1-9]\\d*$/", $_GET['page']) ? $_GET['page'] : 1;
        $page = $page > $pageSize ? 1 : $page;
        $this->set('browse', true);
        $this->set('now', $page);
        $this->set('pageSize', $pageSize);
        $this->set('data', $this->modifyInfo($info->getAllSubmissions($_SESSION['UId'], $page)));
    }

    private function modifyInfo($info){
        if (is_null($info))
            return null;

        $detail = getSystemLanguage() == "zh_CN" ? "查看详情" : "check";

        foreach ($info as $key => $item){
            $item['detail'] = $this->modifyHref($item['SId'], $detail, '/history/submission');
            $item['Title'] = $this->modifyHref($item['PId'], $item['Title'], '/question/pid');
            $item['PId'] = $this->modifyHref($item['PId'], $item['PId'], '/question/pid', "display:block;");
            $item['Status'] = $this->addColor($item['Status']);
            $info[$key] = $item;
        }
        return $info;
    }

    private function modifyHref($id, $text, $site, $extra = ""){
        return "<a href='$site/$id' target='_blank' style='$extra'>$text</a>";
    }

    private function addColor($status){
        if ($status == "AC"){
            $color = "limegreen";
        }else if ($status == 'testing'){
            $color = "darkgray";
        }else
            $color = "#ff0000";

        return "<span style='color: $color'>$status<span>";
    }
}