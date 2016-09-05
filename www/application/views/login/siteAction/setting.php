<?php
/**
 * Author: Alrash
 */?>
<form>
    <!--语言修改-->
    <div style="border-bottom: 1px solid #dddddd; line-height: 3em">
        <div class="choice"><b><?php //echo $site['setting']['languageTitle']?></b></div>
        <span><?php echo $site['setting']['language']?>&nbsp;&nbsp;</span>
        <select>
            <?php
            $langArray = array('system' => 'default', 'zh_CN' => '中文（简体）', 'en' => 'English');
            $lang = isset($_COOKIE['LANG']) ? $_COOKIE['LANG'] : 'system';
            //产成选项
            foreach ($langArray as $value => $show)
            {
                $selected = $lang == $value ? 'selected' : '';
                echo "<option value='$value' $selected> $show </option>";
            }
            ?>
        </select>
        <p style="display: inline-block; font-size: 0.8em; color: #aaaaaa; margin: 0 5em">*<?php echo $site['setting']['languageNote']?></p>
    </div>
    <!--文本信息修改-->
    <div>
        <div class="choice" style="margin-top: -1.5em;"><b><?php //echo $site['setting']['base']?></b></div>
        <ul>
            <li>
                <span><?php echo $site['setting']['nickname']?></span>
                <input value="<?php echo $_SESSION['Nickname']?>">
            </li>
            <li>
                <span><?php echo $site['setting']['email']?></span>
                <input value="<?php echo $_SESSION['Email']?>">
            </li>
            <li>
                <span style="position: absolute"><?php echo $site['setting']['note']?></span>
                <textarea cols="60" rows="8" style="margin-top: 0.4em" placeholder="<?php echo $site['setting']['noteArea']?>"><?php echo $_SESSION['Note']?></textarea>
            </li>
        </ul>
    </div>
    <!--保存/取消专用-->
    <div class="settingAhref" style="border-bottom: 1px solid #dddddd;">
        <a><?php echo $site['setting']['submitUsual']?></a>
    </div>
    <!--头像修改-->
    <div style="border-bottom: 1px solid #dddddd; height: 200px; line-height: 200px">
        <div class="choice"><b><?php //echo $site['setting']['avatar']?></b></div>
        <img src="<?php echo $_SESSION['Image']?>" style="border-radius: 50%; height: 128px; width: 128px; margin: 36px 4em;">
        <div id="Avatar">
            <image style="height: 128px; width: 128px;"><button>上传图片</button></image>
        </div>
    </div>
    <!--密码修改栏-->
    <div style="padding-right: 16em; text-align: center">
        <div class="choice" style="margin-top: -1.5em"><b><?php //echo $site['setting']['password']?></b></div>
        <ul>
            <li>
                <input type="password" placeholder="<?php echo $site['setting']['old']?>">
            </li>
            <li>
                <input type="password" placeholder="<?php echo $site['setting']['new']?>">
            </li>
            <li>
                <input type="password" placeholder="<?php echo $site['setting']['confirm']?>">
            </li>
        </ul>
    </div>
    <!--密码修改专用-->
    <div class="settingAhref">
        <a style="letter-spacing: 0"><?php echo $site['setting']['submitPassword']?></a>
    </div>
</form>
