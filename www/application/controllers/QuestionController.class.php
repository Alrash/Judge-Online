<?php
/**
 * Author: Alrash
 * 用于对题目的处理
 */

class QuestionController extends Controller{
    
    function __construct($controller, $action){
        parent::__construct($controller, $action);
        $this->set('bodyStyle', 'browse');
        $this->set('LOGO', false);
    }

    function index(){
        $this->browse();
    }
    
    function browse(){
        $this->set('Title', 'browse');
        $this->set('browse', true);
        $info = new QuestionModel();
        $page = 1;
        if (isset($_GET['page']) && preg_match('/^[1-9]\d*$/', $_GET['page'])){
            $page = $_GET['page'];
        }
        $this->set('data', $info->getQuestion($page));
        $this->set('pageSize', floor($info->getRecordSize() / RECORD_SIZE + 1));
        $this->set('now', $page);
    }
    
    function recommend(){
        $this->set('Title', 'recommend');
    }

    function history($pid){
        $info = new QuestionModel();

        $this->set('pid', $pid);
        $this->set('MENU', false);
        $this->set('Title', 'history');
        $this->set('lastSix', $this->modifyLastSixHour($info->getRecentSubmission($pid)));
        $this->set('data', $this->modifyHistory($info->getQuestionTop20($pid)));
    }

    private function modifyHistory($info){
        if (is_null($info))
            return;

        foreach ($info as $key => $item){
            $item['Runtime'] = $this->modifyRuntime($item['Runtime']);
            $item['Runmemory'] = $this->modifyRunmemory($item['Runmemory']);
            $item['example'] = $this->modifySId($item['SId']);
            $info[$key] = $item;
        }
        return $info;
    }

    private function modifyLastSixHour($info){
        if (is_null($info))
            return;

        foreach ($info as $key => $item){
            $item['example'] = $this->modifySId($item['SId']);
            $item['time'] = $this->modifyTime($item['time']);
            $info[$key] = $item;
        }
        return $info;
    }

    private function modifyRuntime($time){
        return number_format($time / 1000, 3) . "s";
    }

    private function modifyRunmemory($memory){
        $size = array('B', 'KB', 'MB');
        for ($i = 0; floor($memory / pow(1024, $i + 1)) != 0 && $i < 3; $i++);
        return number_format($memory / pow(1024, $i), 3) . $size[$i];
    }

    private function modifySId($sid){
        return "<a href='/history/submission/$sid' target='_blank' style='display: block'>$sid</a>";
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

    function quickSubmit(){
        $this->submit(1);
        $this->set('quick', true);
        $this->set('display', 'block');
        $this->set('count', 1);
        $this->set('type', 0);
        $this->set('MENU', false);
        $this->set('Title', 'quickSubmit');
    }

    function quickSearch(){
        $this->set('Title', 'quickSearch');
        $this->set('MENU', false);
    }

    function submit($id){
        $this->set('quick', false);

        $question = new QuestionModel();
        $choice = array();
        $xml = $question->getCompilerChoice()->Element;

        foreach ($xml as $key => $item){
            $array = (array)$item;
            $choice[$array['choice']] = $array['compiler'];
        }
        $this->set('compiler', $choice);
    }

    function pid($id = null){
        $this->set('Title', 'pid');
        $this->set('MENU', false);
        $this->set('display', 'none');
        if(is_null($id)) {
            $this->index();
            return;
        }else {
            $this->showDetail($id);
            $this->submit($id);
            $this->set('pid', $id);
        }
    }

    function showDetail($id){
        $question = new QuestionModel;
        $detail = $question->getDetailInformation($id);
        if (is_null($detail)){
            $this->set('pidExist', false);
        }else{
            $detail['note'] = preg_replace('/((http|ftp|https):\/\/[\w-]+(\.[\w-]+)+([\w.,@?^=%&amp;:\/~+#-]*[\w@?^=%&amp;\/~+#-])?)/',
                '<a href="\1" target="_blank">\1</a>', $detail['note']);
            $this->set('pidExist', true);
            foreach ($detail as $key => $value){
                $this->set($key, $value);
            }
            $hard = $question->getHardMode($id);
            $this->set('hardMode', $hard == null ? 3 : $hard[0]['Hard']);
            $this->set('sTitle', $detail['type'] == 0 ? 'fixed' : 'blank');
        }
    }

    function __destruct(){
        $this->_view->specialRenderWithControllerName('question');
    }
}
