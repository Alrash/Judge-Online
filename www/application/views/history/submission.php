<?php
/**
 * Author: Alrash
 * Date: 2016/08/29 16:25
 * Description:
 */
if ($existed == false){
    header("Location: /history/user");
}
?>
<div class="background">
    <div class="centerArea">
        <?php echo $ini['title']?>
        <hr>
        <?php echo '';?>
        <hr>
        源码：
        <div class="codeArea">
            <ol class="code">
                <?php
                $className = array("code01", "code02");
                if (isset($content['source'])){
                    for ($i = 0; $i < sizeof($content['source']); $i++){
                        echo "<li class='". $className[$i % 2] . "'><pre>" . $content['source'][$i] . "</pre></li>";
                    }
                }else {
                    for ($i = 0; $i < sizeof($content); $i++){
                        echo "<li class='". $className[$i % 2] . "'><pre>$content[$i]</pre></li>";
                    }
                }
                ?>
            </ol>
            <?php
            if (isset($content['answer'])){
                echo "<label>" . "具体答案" . "</label>";
                echo "<ol class='code'>";
                for ($i = 0; $i < sizeof($content['answer']); $i++){
                    echo "<li class='". $className[$i % 2] . "'><pre>" . $content['answer'][$i] . "</pre></li>";
                }
                echo "</ol>";
            }
            ?>
        </div>
        <hr>
        编译提示框
        <hr>
        测试
        <?php
        for ($i = 1; $i <= $ini['testFile']; $i++){
            $filename = sprintf("input%02d.txt", $i);
            echo "<a href='/file/input/" . $ini['pid'] . "/$i'>$filename</a>";
        }
        ?>
    </div>
</div>