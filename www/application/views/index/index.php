<div class="search">
    <i id="isearch"></i>
    <input id="search" placeholder="<?php echo $search['search']?>">
    <button id="bsearch"><?php echo $button['search']?></button>
</div>
<?php
foreach ($_SESSION as $key=> $value)
    echo $key . " " . $value . "#<br>";

foreach ($_GET as $key=> $value)
    echo $key . " " . $value . "#<br>";
include APP_PATH . "/script/lib/phpass/PasswordHash.php";
$hasher = new PasswordHash(8, false);
//echo $hasher->HashPassword("cm2544843034LL");
echo $hasher->CheckPassword("cm2544843034LL", '$2a$08$GHuv19g.XL4P4EGN87wu9OekNb5OBmklL.XQcnrgiaEPhaGvj1RVa');