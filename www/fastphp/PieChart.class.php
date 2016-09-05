<?php
/**
 * Author: Alrash
 * Function: use gd model to draw pie chart
 * 注意： 没有释放资源
 */

defined('ANGLE') or define('ANGLE', 75);

class PieChart {
    private $x, $y;             //中心坐标
    private $height, $width;    //图像宽高
    private $d, $l;             //d,l 长度半径
    private $im;                //资源句柄
    private $fontsize = 4;      //字体大小

    /* *
     * 颜色数组，暂时含有九种颜色
     * 创建颜色句柄使用
     */
    protected $colorRGB = array(
        'orange' => array(0xff, 0xa5, 0),
        'sky blue' => array(0x87, 0xce, 0xeb),
        'light gray' => array(0xd3, 0xd3, 0xd3),
        'blue' => array(0, 0, 0xff),
        'yellow' => array(0xff, 0xff, 0),
        'red' => array(0xff, 0, 0),
        'purple' => array(0x80, 0, 0x80),
        'green' => array(0, 0x80, 0),
        'magenta' => array(0xff, 0, 0xff),
        'black' => array(0, 0, 0),
        'white' => array(0xff, 0xff, 0xff),
    );

    protected $color = array();

    public function __construct($height = 400, $width = 400) {
        $this->height = $height;
        $this->width = $width;
        $this->x = round($height / 2);
        $this->y = round($width / 2);
        $this->d = $this->height;
        $this->l = $this->width;
        $this->im = imagecreatetruecolor($width, $height);
        //$this->im = imagecreate($width, $height);

        foreach ($this->colorRGB as $key => $item){
            $this->color[$key] = imagecolorallocate($this->im, $item[0], $item[1], $item[2]);
        }

        imagefill($this->im, 0, 0, $this->color['white']);
        if (function_exists('imageantialias')){
            imageantialias(true);
        }
    }

    /* *
     * notice: 本函数方法，暂无检测
     * @param data 接收纯数字，暂不接收百分比
     * @param extraData 显示百分比以外的信息
     * @param enable 是否显示信息（包括百分比与）
     *
     * @return void: please use getImageResource function to get im variable
     * */
    public function drawPieChart($data = array(), $enable = false, $extraData = array()){
        //计算总值
        $total = 0;
        foreach ($data as $key => $num){
            $total += $num;
        }

        //求百分比
        for($i = 0; $i < sizeof($data); $i++){
            $data[$i] = $data[$i] / $total;
        }

        //获得角度，作为下用参数
        $angleArray = array();
        array_push($angleArray, array(ANGLE, round(ANGLE + 360 * $data[0]) % 360));
        for ($i = 1; $i < sizeof($data); $i++){
            $angleArray[$i] = array($angleArray[$i - 1][1], round($angleArray[$i - 1][1] + $data[$i] * 360) % 360);
        }
        $angleArray[sizeof($angleArray) - 1][1] = ANGLE;

        for ($i = 0; $i < sizeof($data); $i++){
            if ($angleArray[$i][0] != $angleArray[$i][1]){
                imagefilledarc($this->im, $this->x, $this->y, $this->d, $this->l, $angleArray[$i][0], $angleArray[$i][1],
                    $this->color[array_keys($this->color)[$i]], IMG_ARC_PIE | IMG_ARC_EDGED);
            }
        }

        /* *
         * 输出百分比，0不输出
         */
        for ($i = 0; $i < sizeof($data); $i++){
            if ($data[$i] != 0){
                $dis = (($angleArray[$i][1] - $angleArray[$i][0] + 360) % 360) / 2;
                imagestring($this->im, $this->fontsize,
                    $this->x + cos(deg2rad($angleArray[$i][0] + $dis)) * $this->d / 4 - 10,
                    $this->y + sin(deg2rad($angleArray[$i][0] + $dis)) * $this->l / 4,
                    round($data[$i] * 10000) / 100 . "%", $this->color['black']);
            }
        }

        if ($enable){
            for ($i = 0, $line = 0, $column = 0; $i < sizeof($extraData); $i++){
                if ($i % 5 == 0 && $i != 0) {
                    $column++;
                    $line = 0;
                }
                $key = array_keys($this->color)[$i];
                $this->drawFillRectangle(array(5 + 60 * $column, $this->d + 2 + 15 * $line), 10, 5, array($key => $this->colorRGB[$key]));
                imagestring($this->im, 2, 5 + 60 * $column + 10, $this->d + 15 * $line, $extraData[$i], $this->color['black']);
                $line++;
            }
        }
    }

    public function drawFillRectangle($start =array(), $height, $width, $color){
        $this->drawRectangle($start, $height, $width, $height, 0, $width, 0, $color);
    }

    public function drawRectangle($start = array(), $height, $width, $top, $left, $bottom, $right, $color = array()){
        if ($color != array()){
            $key = array_keys($color)[0];
            $this->color[$key] = imagecolorallocate($this->im, $color[$key][0], $color[$key][1], $color[$key][2]);
        }else{
            $key = array_keys($this->color)[0];
        }

        $top = min($top, $height);
        $bottom = min($bottom, $height);
        $left = min($left, $width);
        $right = min($right, $width);

        for ($i = 0; $i < $top; $i++){
            imageline($this->im, $start[0], $start[1] + $i, $start[0] + $width, $start[1] + $i, $this->color[$key]);
        }

        for ($i = 0; $i < $bottom; $i++){
            imageline($this->im, $start[0], $start[1] + $height - $i, $start[0] + $width, $start[1] + $height - $i, $this->color[$key]);
        }

        for ($i = 0; $i < $left; $i++){
            imageline($this->im, $start[0] + $i, $start[1], $start[0] + $i, $start[1] + $height, $this->color[$key]);
        }

        for ($i = 0; $i < $right; $i++){
            imageline($this->im, $start[0] + $width - $i, $start[1], $start[0] + $width - $i, $start[1] + $height, $this->color[$key]);
        }
    }

    /* *
     * function: set center pos, if use this f unction.
     *           Otherwise, use the half of height and width which are set in construct.
     * @param (x, y)
     * @return void
     */
    public function setCenterPos($x, $y){
        $this->x = $x;
        $this->y = $y;
    }

    /* *
     * function: set pie radius, if use this function.
     *           Otherwise, use the half of height and width which are set in construct.
     * @param (d, l)
     * @return void
     */
    public function setRadius($d, $l){
        $this->d = $d;
        $this->l = $l;
    }

    public function setFontsSize($fontsSize){
        $this->fontsize = $fontsSize;
    }

    public function getImageResource(){
        return $this->im;
    }

    public function __destruct() {
        // TODO: Implement __destruct() method.
        //imagedestroy($this->im);
    }
}