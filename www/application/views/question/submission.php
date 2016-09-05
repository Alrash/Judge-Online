<?php
/**
 * Author: Alrash
 */
//可能这个是不需要的，好像原来打算是在提示提交成功的地方，完成的
/*if (!isset($_SESSION['signIn']) || $_SESSION['signIn'] == 0){
    echo "<script text='javascript'> alert('" . $question['right']['submit']['login'] ."'); window.location.href = '/login/signIn'</script>";
}*/
?>
<div class="submission" style="display: <?php echo $display?>">
    <!--弹窗内容-->
    <!--实际内容-->
    <?php
    if ($quick) {
    ?>
        <div class="sPid" id="sPid">
            <span>#</span><input><span class="error"></span>
        </div>
    <?php
    }else {
    ?>
        <!--关闭按钮-->
        <div class="close"><span>X</span></div>
        <div class="sTitle"><?php echo $question['right']['submit'][$sTitle]?></div>
        <div class="sPid" id="sPid"><?php echo "#$pid"?><span class="error"></span></div>
    <?php
    }
    ?>
    <div class="sCompiler">
        <ul id="tab">
            <?php
            foreach ($compiler as $key => $item){
                echo "<li>$key</li>";
                $status[$key] = 'hide';
            }
            ?>
        </ul>
        <div id="content">
            <?php
            $status[key($status)[0]] = 'show';
            foreach ($compiler as $key => $item){
                echo "<div class=$status[$key]><p>$item</p></div>";
            }
            ?>
        </div>
    </div>
    <div class="code" style="overflow: <?php echo $type == null ? 'visible' : ($type == 0 ? 'visible' : 'auto')?>">
        <?php
        $type == null ? 0 : $type;
        for ($i = 0; $i < $count; $i++) {
            if ($type == 0){
                echo "<textarea></textarea>";
            }else {
                echo "<input placeholder=" . $question['right']['placeholder'] . ">";
            }
        }?>
    </div>
    <div class="button"><a id="buttonSubmit"><?php echo $question['right']['submit']['buttonSubmit']?></a></div>
</div>
<div class="overlay"></div>
