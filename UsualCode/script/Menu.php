<?php
    require_once('./script/lib/Number.php');
?>
<div class="usermenu">
    <!--UserMenu-->
    <div id="info">
        <?php
            if (isset($_SESSION['Signin']) && $_SESSION['Signin'])
            {//已经登录的情况
                $dis_login = 'block';
                $dis_logout = 'none';
            }
            else
            {//还没有登录
                $dis_login = 'none';
                $dis_logout = 'block';
            }
        ?>
        <div class="login" style="display: <?php echo $dis_login;?>">
            <ul>
                <div id="img_over">
                    <ul id="i_face">
                        <a href="#">
                            <img id="over" src="<?php echo 'http://localhost/PhpStorm/Test' . $_SESSION['Image'];?>"/>
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
                            <li><?php echo $usermenu['accept' . $lang] . ' : ' . $_SESSION['Right'];?></li>
                            <li><?php echo $usermenu['amount' . $lang] . ' : ' . $_SESSION['Total'];?></li>
                        </div>
                    </ul>
                </div>
                <div id="leftmenu">
                    <li>
                        <a href="#"><?php echo $usermenu['submit' . $lang]?></a>
                    </li>
                    <li>
                        <a href="#"><?php echo $usermenu['recommend' . $lang]?></a>
                    </li>
                    <li>
                        <a href="#"><?php echo $usermenu['out' . $lang]?></a>
                    </li>
                </div>
            </ul>
        </div>
        <div class="logout" style="display: <?php echo $dis_logout;?>">
            <ul>
                <li>
                    <a href="Login.php"><?php echo $usermenu['lanch' . $lang]?></a>
                </li>
                <li id="split">|</li>
                <li>
                    <a href="Register.php"><?php echo $usermenu['reg' . $lang]?></a>
                </li>
            </ul>
        </div>
    </div>
    <div class="usermenu_background"></div>
</div>

<div class="header">
    <!--Header-->
    <a href="./index.php" title="<?php echo $menu['Index' . $lang]; ?>" style="float:left;"></a>
</div>

<div class="navigation">
    <!--Navigation-->
    <ul>
        <li id="Index">
            <a href="./index.php"><?php echo $menu['Index' . $lang]; ?></a>
        </li>
        <li id="Browse">
            <a href="#"><?php echo $menu['Browse' . $lang]; ?></a>
        </li>
        <li id="Recommend">
            <a href="#"><?php echo $menu['Recommend' . $lang]; ?></a>
        </li>
        <li id="Data">
            <a href="#"><?php echo $menu['Data' . $lang]; ?></a>
        </li>
        <li id="Help">
            <a href="#"><?php echo $menu['Help' . $lang]?></a>
        </li>
    </ul>
</div>