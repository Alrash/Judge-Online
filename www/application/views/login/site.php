<?php
/**
 * Author: Alrash
 * 用于显示个人信息
 * */
?>
<div id="information">
    <div id="infoArea">
        <div class="left_area">
            <ul>
                <span><?php echo $site['menu']['title']?></span>
                <!--公开的信息，包括回答的题数-->
                <li id="<?php echo isset($index) ? $index : '';?>">
                    <a href="/site"><?php echo $site['menu']['index']?></a>
                </li>
                <!--各种修改-->
                <li id="<?php echo isset($setting) ? $setting : '';?>">
                    <a href="/site/setting"><?php echo $site['menu']['setting']?></a>
                </li>
                <!--提交的信息总汇-->
                <li id="<?php echo isset($submission) ? $submission : '';?>">
                    <a href="/site/submission"><?php echo $site['menu']['submission']?></a>
                </li>
                <!--上传题目使用-->
                <li id="<?php echo isset($upQuestion) ? $upQuestion : '';?>">
                    <a href="/site/upQuestion"><?php echo $site['menu']['upQuestion']?></a>
                </li>
                <!--修改上传的题目-->
                <li id="<?php echo isset($updateQuestion) ? $updateQuestion : '';?>">
                    <a href="/site/updateQuestion"><?php echo $site['menu']['updateQuestion']?></a>
                </li>
            </ul>
        </div>
        <div class="right_area">
            <!--信息提示-->
            <div id="siteTitle">
                <span id="squareArea">&nbsp;</span>
                <span id="infoText"><?php echo $site['title'][$arrayIndex]?></span>
            </div>
            <div id="realArea">
            <?php
            include APP_PATH . 'application/views/login/siteAction/'. $includePage . '.php';
            ?>
            </div>
        </div>
    </div>
</div>
<div style="height: 100px">
</div>
