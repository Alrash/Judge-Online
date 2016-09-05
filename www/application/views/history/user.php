<?php
/**
 * Author: Alrash
 * Date: 2016/08/29 16:25
 * Description:
 */
if (!isset($_SESSION['signIn']) || !$_SESSION['signIn']){
    echo "<script type='application/javascript'>window.location.href = '/login/signIn'</script>";
}
?>
<style type="text/css">
    .datalist #tableList tr{
        line-height: 26px;
        height: 26px;
    }
</style>
<div class="datalist" style="margin: 2% auto; width: 80%;">
    <div class="dataArea">
        <table id="tableList" style="width: 100%;">
            <tr class="columnTitle" style="line-height: 2em;">
                <td class="submission"><?php echo $history['user']['submission']?></td>
                <td class="question"><?php echo $history['user']['question']?></td>
                <td class="u-title"><?php echo $history['user']['title']?></td>
                <td class="status"><?php echo $history['user']['status']?></td>
                <td class="detail"><?php echo $history['user']['detail']?></td>
            </tr>
            <?php
            if (isset($data)){
                $className = array("cell01", "cell02");
                for ($i = 0; $i < sizeof($data); $i++){
                    echo "<tr class='" . $className[$i % 2] . "'>";
                        echo "<td class='submission'>" . $data[$i]['SId'] . "</td>";
                        echo "<td class='question'>" . $data[$i]['PId'] . "</td>";
                        echo "<td class='u-title'>" . $data[$i]['Title'] . "</td>";
                        echo "<td class='status'>" . $data[$i]['Status'] . "</td>";
                        echo "<td class='detail'>" . $data[$i]['detail'] . "</td>";
                    echo "</tr>";
                }
            }
            ?>
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
</div>
