<?php

/**
 * Author: Alrash
 * Date: 2016/08/30 20:51
 * Description:
 */
class FileController extends Controller{

    public function input($pid, $num){
        $data = null;
        $filename = sprintf("input%02d.txt", $num);
        if (!is_numeric($pid) || !is_numeric($num)){
            return array('filename' => '', 'path' => '');
        }
        return array('filename' => $filename, 'path' => DATA_PATH . "Questions/$pid/in/$filename");
    }

    /**
     * file文件的唯一接口
     * 使用nginx转向时，website/file(这是接口，而非控制器)/实际动作名
     * */
    public function file($realAction = null, $paramOne = null, $paramTwo = null) {
        $data = null;
        if (!method_exists($this, $realAction)) {
            $data['filename'] = 'error.html';
            $data['path'] = '';
        }else {
            $data = $this->$realAction($paramOne, $paramTwo);
        }

        $this->set('filename', $data['filename']);
        $this->set('path', $data['path']);
    }

    function __destruct() {
        // TODO: Implement __destruct() method.
        $this->_view->showDealResult();
    }
}