<?php
/**
 * Author: Alrash
 */
?>
<script type="text/javascript" src="/js/LoginAndRegister.js"></script>
<div class="login">
    <div class="area_log">
        <form id="log_form">
            <h3><?php echo $header['login']?></h3>
            <div class="area">
                <input type="text" name="NameId" id="NameId" placeholder="<?php echo $nickname['login'];?>" required="required">
                <br><span id="Error"></span>
            </div>
            <div class="area">
                <input type="password" name="Passwd" id="PassWd" placeholder="<?php echo $passwd['login']?>" minlength="6" maxlength="20">
            </div>
            <div class="area_log_sub">
                <div class="login_a">
                    <label><input type="checkbox"><?php echo $label['login'];?></label>
                </div>
                <div class="area_log_sub_a">
                    <a id="login"><?php echo $submit['login'];?></a>
                </div>
                <div id="refresh">
                    <span id="hidden"><?php echo $_SESSION['token'];?></span>
                </div>
            </div>
        </form>
        <div class="login_other">
            <div class="login_other_half">
                <a href="#"><?php echo $ahref['forgetPasswd'];?></a>
            </div>
            <div class="login_other_half">
                <a href="/login/register"><?php echo $ahref['regPage'];?></a>
            </div>
        </div>
    </div>
</div>
