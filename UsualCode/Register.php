<?php
    /**
     * Author: Alrash
     * 注册页面
     */
    if (!isset($_SESSION))
        session_start();
    require_once("./script/config/lang.php");
    require_once("./script/language.php");

    //在这里判断用户有无登录
    //若登录，显示已登录，2s后跳转回index.php
?>
<!DOCTYPE html>
<html>
<head>
    <?php
        //载入脚本与meta标签
        include_once('./script/Head.php');
    ?>

    <script src="./script/js/LoginAndRegister.js"></script>
    <title><?php echo $title['reg' . $lang]; ?></title>
    
</head>
<body>
    <?php require_once "./script/Menu.php";//导入导航栏?>
    <div class="register">
        <div class="area_reg">
            <?php //貌似form中action与method属性都没用了 ╮(￣▽￣)╭?>
            <form id="register_form" action="#" method="post">
                <h3><?php echo $header['reg' . $lang];?></h3>
                <div class="area">
                    <input type="text" name="nickname_reg" id="nickname_reg" placeholder="<?php echo $nickname['reg' . $lang]; ?>" maxlength="20" required="required">
                    <!--在这里这样使用span是开始设计的失误-_-|||-->
                    <br><span id="NickErr"></span>
                </div>
                <div class="area">
                    <input type="text" name="email_reg" id="email_reg" placeholder="<?php echo $email['reg' . $lang]; ?>" required="required">
                    <br><span id="EmailErr"></span>
                </div>
                <div class="area">
                    <input type="password" name="passwd_reg" id="passwd_reg" placeholder="<?php echo $passwd['reg' . $lang]; ?>" maxlength="20" required="required">
                    <br><span id="PasswdErr"></span>
                </div>
                <div class="area">
                    <input type="password" name="repasswd_reg" id="repasswd_reg" placeholder="<?php echo $passwd['repwd' . $lang]; ?>" required="required">
                    <br><span id="RepasswdErr"></span>
                </div>
                <div class="area_reg_sub">
                    <a id="submit"><?php echo $submit['reg' . $lang];?></a>
                    <a href="Login.php" id="sigin"><?php echo $ahref['returnSignin' . $lang]?></a>
                    <div id="refresh"><span id="hidden"><?php echo $_SESSION['token'];?></span></div>
                </div>
            </form>
            <div class="success">
                <p id="p_succ">注册成功，即将跳转至首页</p>
                <a id="a_succ" href="index.php">没有跳转？请点击这条连接</a>
            </div>
        </div>
    </div>
</body>
</html>
