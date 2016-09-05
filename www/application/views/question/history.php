<?php
/**
 */
?>
<!--本页存在的问题：
  1. 仅更新了近期提交表，其余数据没有更新
  2. 每5秒更新一次时，页面存在抖动
  #现在解决方法：
  放弃ajax更新方式，暂时直接使用后台输出
 -->
<div style="width: 100%">
    <div style="width: 45%;display: inline-block">
        <div style="width: 250px; margin:0 auto">
            <img src="/image/questionTrueFalse?id=<?php echo $pid?>&height=333&width=250">
        </div>
    </div>
    <div style="width: 45%;display: inline-block">
        <div style="width: 250px; margin:0 auto">
            <img src="/image/questionCompiler?id=<?php echo $pid?>&height=333&width=250">
        </div>
    </div>
</div>
<div class="datalist" style="min-height: 0;width: 100%; margin-bottom: 2em">
    <div class="dataArea">
        <script>
            var pid = window.location.pathname.split("/");
            var className = Array("cell01", "cell02");
            pid = pid[pid.length - 1];

            /* *
             * json格式数据转array
             */
            function to_Array(json){
                var data = eval(json);
                var arr = new Array();

                for (var i = 0; i < data.length; i++){
                    arr[i] = new Array();
                    arr[i].push(i + 1);
                    for (var x in data[i]){
                        arr[i].push(data[i][x]);
                    }
                }

                return arr;
            }

            function clearTable(table) {
                var i = 0;
                list = table.getElementsByTagName("tr");
                while(list.length != 1 && list.length != 0){
                    if (list[i].className != "columnTitle"){
                        list[i].remove();
                        continue;
                    }
                    i++;
                }
            }

            function fillTable(obj, classNmae, table) {
                var tr = document.createElement("tr");
                var childClass = Array("debug", "nickname", "", "", "", "");
                var td;

                tr.className = classNmae;
                for (var i = 0; i < obj.length; i++){
                    td = document.createElement("td");
                    td.className = childClass[i];
                    td.innerHTML = obj[i];
                    tr.appendChild(td);
                }
                table.appendChild(tr);
            }

            function updateTable() {
                var table = document.getElementsByClassName("dataArea")[0].getElementsByTagName("table")[0];
                clearTable(table);

                $.ajax({
                    async: true,
                    type: "get",
                    url: "/info/getRecentSubmission",
                    data: "pid=" + pid,
                    dataType: "json",
                    error:function () {
                        alert("network connected error");
                    },
                    success:function (resopnse) {
                        var data = to_Array(resopnse);
                        clearTable(table);
                        for (var i = 0; i < data.length; i++){
                            fillTable(data[i], className[i % 2], table);
                        }

                        setTimeout(updateTable, 5000);
                    },
                });
            }

            $(document).ready(function () {
                //updateTable();
            });
        </script>
        <label><?php echo $question['right']['history']['label']?></label>
        <table id="tableList" style="width: 100%;">
            <tr class="columnTitle">
                <td class="debug"><?php echo $question['right']['history']['serial']?></td>
                <td class="nickname"><?php echo $question['right']['history']['nickname']?></td>
                <td><?php echo $question['right']['history']['example']?></td>
                <td><?php echo $question['right']['history']['language']?></td>
                <td><?php echo $question['right']['history']['status']?></td>
                <td><?php echo $question['right']['history']['last']?></td>
            </tr>
            <?php
            /* *
             * 貌似这里可以省略了-_-|||
             * 前台更新加载有问题，参见$4(最上面注释)
             */
            if (isset($lastSix)){
                $className = array('cell01', 'cell02');
                for ($i = 0; $i < sizeof($lastSix) && $i < 20; $i++){
                    echo '<tr class="'. $className[$i % 2] . '">';
                        echo "<td class='debug'> " . ($i + 1) . "</td>";
                        echo "<td class='nickname'>". $lastSix[$i]['Nickname'] . "</td>";
                        echo "<td>" . $lastSix[$i]['example'] . "</td>";
                        echo "<td>" . $lastSix[$i]['compiler'] . "</td>";
                        echo "<td>" . $lastSix[$i]['Status'] . "</td>";
                        echo "<td>" . $lastSix[$i]['time'] . "</td>";
                    echo "</tr>";
                }
            }
            ?>
        </table>
    </div>
</div>
<div class="datalist" style="min-height: 0;width: 100%; margin: 2em 0">
    <div class="dataArea">
        <label><?php echo $question['right']['history']['rank']?></label>
        <table id="tableList" style="width: 100%">
            <tr class="columnTitle">
                <td class="debug"><?php echo $question['right']['history']['serial']?></td>
                <td class="nickname"><?php echo $question['right']['history']['nickname']?></td>
                <td><?php echo $question['right']['history']['example']?></td>
                <td><?php echo $question['right']['history']['language']?></td>
                <td><?php echo $question['right']['history']['time']?></td>
                <td><?php echo $question['right']['history']['memory']?></td>
            </tr>
            <?php
            if (isset($data)){
                $className = array('cell01', 'cell02');
                for ($i = 0; $i < sizeof($data) && $i < 20; $i++){
                    echo '<tr class="'. $className[$i % 2] . '">';
                        echo "<td class='debug'> " . ($i + 1) . "</td>";
                        echo "<td class='nickname'>". $data[$i]['Nickname'] . "</td>";
                        echo "<td>" . $data[$i]['example'] . "</td>";
                        echo "<td>" . $data[$i]['compiler'] . "</td>";
                        echo "<td>" . $data[$i]['Runtime'] . "</td>";
                        echo "<td>" . $data[$i]['Runmemory'] . "</td>";
                    echo "</tr>";
                }
            }
            ?>
        </table>
    </div>
</div>
