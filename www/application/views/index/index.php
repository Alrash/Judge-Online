<script type="application/javascript" src="/js/search.js"></script>
<div class="search">
    <i id="isearch"></i>
    <input id="search" placeholder="<?php echo $search['search']?>">
    <button id="bsearch"><?php echo $button['search']?></button>
    <div class="dropMenu">
        <ul id="listSearch"></ul>
    </div>
</div>
<?php

foreach ($_SESSION as $key=> $value)
    echo $key . " " . $value . "#<br>";

foreach ($_GET as $key=> $value)
    echo $key . " " . $value . "#<br>";
