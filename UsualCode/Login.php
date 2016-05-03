<?php
/**
 * Author: Alrash
 * 用于登录使用
 * 进入此页的标准：没有登录信息(session或cookie没有存储)
 */
include("./script/lib/String.php");

if (!isset($_SESSION))
{
    session_start();
    $_SESSION['token'] = md5(getSpecialRandString(6, 16));
}

require_once("./script/config/lang.php");
require_once("./script/language.php");
?>
<!DOCTYPE html>
<html>
<head>
    <?php include_once('./script/Head.php');?>
    <script type="text/javascript" src="./script/js/LoginAndRegister.js"></script>
    <title><?php echo $title['login' . $lang];?></title>
</head>
<body>
    <?php require_once('./script/Menu.php');?>
    <div class="login">
        <div class="area_log">
            <form id="log_form">
                <h3><?php echo $header['login' . $lang]?></h3>
                <div class="area">               
                    <input type="text" name="NameId" id="NameId" placeholder="<?php echo $nickname['login' . $lang];?>" required="required">
                    <br><span id="Error"></span>
                </div>
                <div class="area">
                    <input type="password" name="Passwd" id="PassWd" placeholder="<?php echo $passwd['login' . $lang]?>" minlength="6" maxlength="20">
                </div>
                <div class="area_log_sub">
                    <div class="login_a">
                        <label><input type="checkbox"><?php echo $label['login' . $lang];?></label>
                    </div>
                    <div class="area_log_sub_a">
                        <a id="login"><?php echo $submit['login' . $lang];?></a>
                    </div>
                    <div id="refresh">
                        <span id="hidden"><?php echo $_SESSION['token'];?></span>
                    </div>
                </div>
            </form>
            <div class="login_other">
                <div class="login_other_half">
                    <a href="ForgetPasswd.php"><?php echo $ahref['forgetPasswd' . $lang];?></a>
                </div>
                <div class="login_other_half">
                    <a href="Register.php"><?php echo $ahref['regPage' . $lang];?></a>
                </div>
            </div>
        </div>
    </div>
</body>
</html>
