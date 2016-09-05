<?php
/**
 */
?>
<script type="text/javascript" src="/js/dropMenu.js"></script>
<div class="datalist">
    <div class="filter">
        <div class="choice">
            <span class="star" style="color: #ffd500" name="select">★</span>
            <span class="star" style="color: #dadddd" name="clear">★</span>
        </div>
        <input type="checkbox" value="1" class="checkboxIn">
        <span class="star">★</span>
        <input type="checkbox" value="2" class="checkboxIn">
        <span class="star">★★</span>
        <input type="checkbox" value="3" class="checkboxIn">
        <span class="star">★★★</span>
        <input type="checkbox" value="4" class="checkboxIn">
        <span class="star">★★★★</span>
        <input type="checkbox" value="5" class="checkboxIn">
        <span class="star">★★★★★</span>
        <br><br>
        <input type="checkbox" value="100" class="checkboxIn">
        <span><?php echo $question['right']['submit']['fixed']?></span>
        <input type="checkbox" value="101" class="checkboxIn">
        <span><?php echo $question['right']['submit']['blank']?></span>
    </div>
    <div class="search" style="margin: 3% 10%">
        <i id="isearch"></i>
        <input id="search">
        <button id="bsearch"><?php echo $button['search']?></button>
        <div class="dropMenu" style="width: 65%">
            <ul id="listSearch" style="background: #fff"></ul>
        </div>
    </div>
    <?php include "showQuestion.php"?>
</div>
