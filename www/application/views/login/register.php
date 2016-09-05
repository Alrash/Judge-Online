<?php
/**
 * Author: Alrash
 */
?>
<script type="text/javascript" src="/js/LoginAndRegister.js"></script>
<div class="register">
    <div class="area_reg">
        <?php //貌似form中action与method属性都没用了 ╮(￣▽￣)╭?>
        <form id="register_form" action="#" method="post">
            <h3><?php echo $header['reg'];?></h3>
            <div class="area">
                <input type="text" name="nickname_reg" id="nickname_reg" placeholder="<?php echo $nickname['reg']; ?>" maxlength="20" required="required">
                <!--在这里这样使用span是开始设计的失误-_-|||-->
                <br><span id="NickErr"></span>
            </div>
            <div class="area">
                <input type="text" name="email_reg" id="email_reg" placeholder="<?php echo $email['reg']; ?>" required="required">
                <br><span id="EmailErr"></span>
            </div>
            <div class="area">
                <input type="password" name="passwd_reg" id="passwd_reg" placeholder="<?php echo $passwd['reg']; ?>" maxlength="20" required="required">
                <br><span id="PasswdErr"></span>
            </div>
            <div class="area">
                <input type="password" name="repasswd_reg" id="repasswd_reg" placeholder="<?php echo $passwd['repwd']; ?>" required="required">
                <br><span id="RepasswdErr"></span>
            </div>
            <div class="area_reg_sub">
                <a id="submit"><?php echo $submit['reg'];?></a>
                <a href="/login/signIn" id="sigin"><?php echo $ahref['returnSignin']?></a>
                <div id="refresh"><span id="hidden"><?php echo $_SESSION['token'];?></span></div>
            </div>
        </form>
        <div class="success">
            <p id="p_succ">注册成功，即将跳转至首页</p>
            <a id="a_succ" href="/index">没有跳转？请点击这条连接</a>
        </div>
    </div>
</div>
