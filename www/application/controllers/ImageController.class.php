<?php
/**
 * Created by PhpStorm.
 * User: alrash
 * Date: 8/24/16
 * Time: 3:25 PM
 */

class ImageController extends Controller{

    /*
     * for test
     function getImage(){
        $pie = new PieChart(400, 300);
        $pie->setCenterPos(150, 150);
        $pie->setRadius(300, 300);
        $pie->drawPieChart(array(10, 20, 30, 40, 50, 60, 70, 80, 90), true, array("cao", "meng", "cao", "meng", "cao", "meng", "cao", "meng", "cao"));
        return $pie->getImageResource();
    }*/

    public function questionTrueFalse(){
        if (isset($_GET['id']) && is_numeric($_GET['id'])){
            $height = $this->getHeight();
            $width = $this->getWidth();
            $pos = $this->getCenterPos($height, $width);
            $pie = new PieChart($height, $width);
            $database = new ImageModel();

            $pie->setCenterPos($pos[0], $pos[1]);
            $pie->setRadius(min($height, $width), min($height, $width));
            $pie->drawPieChart($database->getRightWrongSize($_GET['id']), true, array("Right", "Wrong"));
            $im = $pie->getImageResource();
        }else{
            $im = @imagecreatetruecolor(120, 20) or die('Cannot Initialize new GD image stream');
            $text_color = imagecolorallocate($im, 233, 14, 91);
            imagefill($im, 0, 0, imagecolorallocate($im, 0xff, 0xff, 0xff));
            imagestring($im, 1, 5, 5,  'not match pid', $text_color);
        }
        return $im;
    }

    /* *
     * 全0时，输出有问题
     * 原因还是在draw方法中的百分比
     */
    public function questionCompiler(){
        if (isset($_GET['id']) && is_numeric($_GET['id'])){
            $height = $this->getHeight();
            $width = $this->getWidth();
            $pos = $this->getCenterPos($height, $width);
            $pie = new PieChart($height, $width);
            $database = new ImageModel();

            $pie->setCenterPos($pos[0], $pos[1]);
            $pie->setRadius(min($height, $width), min($height, $width));
            $pie->drawPieChart($database->getCompiler($_GET['id']), true, array("C", "C++", "C++11", "Java", "Python3.5"));
            $im = $pie->getImageResource();
        }else{
            $im = @imagecreatetruecolor(120, 20) or die('Cannot Initialize new GD image stream');
            $text_color = imagecolorallocate($im, 233, 14, 91);
            imagefill($im, 0, 0, imagecolorallocate($im, 0xff, 0xff, 0xff));
            imagestring($im, 1, 5, 5,  'not match pid', $text_color);
        }
        return $im;
    }

    private function getHeight(){
        return isset($_GET['height']) && is_numeric($_GET['height']) ? $_GET['height'] : 400;
    }

    private function getWidth(){
        return isset($_GET['width']) && is_numeric($_GET['width']) ? $_GET['width'] : 300;
    }

    private function getCenterPos($height, $width){
        $tmp = min($height, $width);
        return array(round($tmp / 2), round($tmp / 2));
    }

    /**
     * 图像生成模块的唯一接口
     * 使用nginx转向时，website/image(这是接口，而非控制器)/实际动作名
     * */
    public function image($realAction = null) {
        $im = null;
        if (!method_exists($this, $realAction)) {
            $im = imagecreatetruecolor(100, 10);
            imagefill($im, 0, 0, imagecolorallocate($im, 0xff, 0xff, 0xff));
        }else {
            $im = $this->$realAction();
        }

        $this->set('im', $im);
    }

    function __destruct() {
        // TODO: Implement __destruct() method.
        $this->_view->showDealResult();
    }
}