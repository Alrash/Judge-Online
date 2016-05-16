<?php
/**
 * Author: Alrash
 * 用于显示和提交反馈意见
 * 勿吐嘈如此奇葩的缩进，页面强迫症没救了-_-|||
 */
?>
        <div id="showSuggest">
            <?php echo isset($Submit) ? $Submit : false;?>
            <?php
            $xml = simplexml_load_file(DATA_PATH . 'aboutSuggest.xml'); 
            print_r($xml);
            echo '<br><br>';
            ?>
            <?php
            var_dump($data);
            ?>
        </div>
        <hr>
        <div id="suggest">
            <form action="/about/suggest" method="post">
                <textarea name="suggest" rows="5"></textarea>
                <br><input type="submit" value="发表">
            </form>
        </div>