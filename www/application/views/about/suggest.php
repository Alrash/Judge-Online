<?php
/**
 * Author: Alrash
 * 用于显示和提交反馈意见
 * 勿吐嘈如此奇葩的缩进，页面强迫症没救了-_-|||
 */
?>
        <div id="showSuggest">
            <?php
            //这里比较乱 orz
            foreach (array_reverse($data) as $key => $value)
            {
                echo "<div class=styleSuggest>\n";
                echo    "<div class=styleSuggestLeft>\n";
                echo        "<img src='$value[Image]'>";
                echo        $value["Nickname"] . "<br>" ;
                echo    "</div>\n";
                echo    "<div class=styleSuggestRight>\n";
                echo        $value["suggest"];
                echo    "</div>\n";
                if (isset($_SESSION['Nickname']) && isset($_SESSION['Trust']) && 
                    ($_SESSION['Trust'] > 0 || $_SESSION['Nickname'] == $value['Nickname']))
                    echo '<a href="http://115.159.158.228">delete</a>';
                echo "</div>\n
                     <hr>\n";
            }
            ?>
        </div>
        <div id="suggest">
            <form action="/about/suggest" method="post">
                <textarea name="suggest" rows="5" placeholder="<?php echo $about['right']['suggestArea']?>" required="required"></textarea>
                <br><input type="submit" value="发表">
            </form>
        </div>