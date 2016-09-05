<?php
    require_once SCRIPT_PATH . '/lib/Number.php';
?>
<div class="usermenu">
    <!--UserMenu-->
    <div class="left">
        <a href="/index"><?php echo $usermenu['index']?></a>
    </div>
    <div id="info">
        <?php
        if (isset($_SESSION['signIn']) && $_SESSION['signIn'])
        {//已经登录的情况
            $dis_login = 'block';
            $dis_logout = 'none';
        ?>
        <div class="login" style="display: <?php echo $dis_login;?>">
            <ul>
                <div id="img">
                    <ul id="i_face">
                        <a href="/site">
                            <img id="usual" src="<?php echo $_SESSION['Image'];?>"/>
                        </a>
                    </ul>
                    <ul id="dropdown">
                        <div id="name">
                            <b><?php echo $_SESSION['Nickname'];?></b>
                        </div>
                        <div>
                            <li>Lv: <?php echo getLevel($_SESSION['Exp']);?></li>
                            <li>Exp: <?php echo getExp($_SESSION['Exp']);?></li>
                        </div>
                        <div>
                            <li><?php echo $usermenu['accept'] . ' : ' . $_SESSION['Right'];?></li>
                            <li><?php echo $usermenu['amount'] . ' : ' . $_SESSION['Total'];?></li>
                        </div>
                    </ul>
                </div>
                <div id="leftmenu">
                    <li>
                        <a href="/question/quickSearch"><?php echo $usermenu['submit']?></a>
                    </li>
                    <li>
                        <a href="/question/recommend"><?php echo $usermenu['recommend']?></a>
                    </li>
                    <li>
                        <a href="/history/user"><?php echo $usermenu['history']?></a>
                    </li>
                    <li>
                        <a href="#"><?php echo $usermenu['out']?></a>
                    </li>
                </div>
            </ul>
        </div>
        <?php
        }
        else
        {//还没有登录
            $dis_login = 'none';
            $dis_logout = 'block';
        ?>
        <div class="logout" style="display: <?php echo $dis_logout;?>">
            <ul>
                <li>
                    <a href="/login/signIn"><?php echo $usermenu['launch']?></a>
                </li>
                <li id="split">|</li>
                <li>
                    <a href="/login/register"><?php echo $usermenu['reg']?></a>
                </li>
            </ul>
        </div>
        <?php
        }
        ?>
    </div>
    <div class="usermenu_background"></div>
</div>

<?php
if ($LOGO) {
?>
<div class="header">
    <div class="backgroundImage">
    <!--Header-->
    <a href="/index" title="<?php echo $menu['Index']; ?>" style="float:left;"></a>
    </div>
</div>
<?php
}
?>
<?php
if ($MENU) {
?>
<div class="navigation">
    <!--Navigation-->
    <ul>
        <li id="Index">
            <a href="/index"><?php echo $menu['Index']; ?></a>
        </li>
        <li id="Browse">
            <a href="/question"><?php echo $menu['Browse']; ?></a>
        </li>
        <li id="Recommend">
            <a href="#"><?php echo $menu['Recommend']; ?></a>
        </li>
        <li id="Data">
            <a href="#"><?php echo $menu['Data']; ?></a>
        </li>
        <li id="Help">
            <a href="/about"><?php echo $menu['Help']?></a>
        </li>
    </ul>
</div>
<?php
}
?>