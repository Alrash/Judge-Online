<?php
/**
 */
?>
<div class="dataArea">
    <table id="tableList">
        <tr class="columnTitle">
            <td class="debug"></td>
            <td class="title"><?php echo $question['right']['table']['title']?></td>
            <td class="type"><?php echo $question['right']['table']['type']?></td>
            <td class="count"><?php echo $question['right']['table']['count']?></td>
            <td class="per"><?php echo $question['right']['table']['per']?></td>
            <td class="hard"><?php echo $question['right']['table']['hard']?></td>
            <?php
            if (isset($browse) && $browse){
                $className = Array('cell01', 'cell02');
                $percent = 0;
                for ($i = 0; $i < sizeof($data); $i++){
                    echo "<tr class='" .  $className[$i % 2]. "'>";
                        echo "<td class='debug'><a href='/question/debug/" . $data[$i]['PId'] . "'><i></i></a></td>";
                        echo "<td class='title'><a href='/question/pid/" . $data[$i]['PId'] . "' target='_blank'>" . $data[$i]['Title'] . "</a></td>";
                        echo "<td class='type'>" . $data[$i]['Type'] . "</td>";
                        echo "<td class='count'>" . $data[$i]['Total'] . "</td>";
                        $percent = is_null($data[$i]['Per']) ? 0 : $data[$i]['Per'];
                        echo "<td class='per'>" .
                                "<div class='backgroundTable'> " .
                                    "<div class='right' style='width: " . $percent . "%'>...</div>" .
                                    "<div class='showText'>" . $percent . "%</div>" .
                            "</td>";
                        echo "<td class='hard'>" . $data[$i]['Hard'] . "</td>";
                    echo "</tr>";
                }
            }
            ?>
        </tr>
    </table>
    <div class="page">
        <?php
        if (isset($browse) && $browse) {
            ?>
            <script type="text/javascript">
                $(document).ready(function () {
                    $("#skip").on("keypress", function () {
                        if (event.keyCode != 13)
                            return;
                        var page = document.getElementById("skip").value.trim();
                        if (page.match(/^[1-9]\d*$/)) {
                            window.location.search = "page=" + page;
                        }
                    });
                });
            </script>
            <?php
        }?>
        <div class="pageList">
            <ul id="pageUL">
                <li id="font"><a href="?page=1"><?php echo $question['right']['page']['font']?></a></li>
                <?php
                if (isset($browse) && $browse){
                    $start = 1;
                    $end = 8;
                    if ($now < 5){
                        $end = min($end, $pageSize) + 1;
                    }else if ($pageSize - $now > 3){
                        $end = $now + 4;
                        $start = $now - 4;    //now - 5 + 1
                    }else {
                        $end = $pageSize + 1;
                        $start = max($end - 8, 1);
                    }

                    for ($i = $start; $i < $end; $i++){
                        if ($i != $now){
                            echo "<li><a href='/question/browse?page=$i'>$i</a></li>";
                        }else {
                            echo "<span>$now</span>";
                        }
                    }
                }
                ?>
                <li id="back"><a href=<?php echo isset($pageSize) && preg_match('/\d+/', $pageSize) ? "?page=" . $pageSize : "" ?>><?php echo $question['right']['page']['back']?></a></li>
            </ul>
            <div class="skip">
                <input name="skip" id="skip" style="width: 10%;text-align: center">
                <span style="margin: 0 5px"><?php echo $question['right']['skip']?></span>
            </div>
        </div>
        <div class="pageFooter" style="height: 24px"></div>
    </div>
</div>
