/**
 * Author: Alrash
 * create time: 2016-08-18
 */

var list;
/* *
 * json格式数据转array
 */
function to_Array(json){
    var data = eval(json);
    var arr = new Array();

    for (var i = 0; i < data.length; i++){
        arr[i] = new Array();
        for (var x in data[i]){
            arr[i].push(data[i][x]);
        }
    }

    return arr;
}

function to_StringA(json, index){
    var data = eval(json);
    var arr = new Array();

    for (var i = 0; i < data.length; i++){
        arr[i] = new Array();
        var tmp = data[i][index[0]] + " - ";
        for (var j = 1; j < index.length; j++){
            tmp += " " + data[i][index[j]];
        }
        arr[i].push(tmp);
    }

    return arr;
}

RegExp.escape = function (s) {
    return RegExp(s.replace(/([-\/\\^$*+?.()|[\]{}])/g, '\\$1'));
};

//最长公共子序列
function LIC(str1,str2){
    var dp = new Array();

    for (var i = 0; i <= str1.length; i++){
        dp[i] = new Array();
        for (var j = 0; j <= str2.length; j++){
            dp[i].push(0);
        }
    }

    for (var i = 0; i < str1.length; i++) {
        for (var j = 0; j < str2.length; j++) {
            if (str1[i] == str2[j])
                dp[i + 1][j + 1] = dp[i][j] + 1;
            else
                dp[i + 1][j + 1] = Math.max(dp[i][j + 1], dp[i + 1][j]);
        }
    }

    return dp[str1.length][str2.length];
}

$(document).ready(function(){
    $("#search").on('focus', function () {
        $.ajax({
            async: true,
            type: "get",
            url: "/info/getInfoWithJson",
            data: "mode=question",
            dataType: "json",
            error:function () {
            },
            success:function (response) {
                list = to_StringA(response, new Array('PId', 'Title'));
            }
        });
    });

    $("#search").on("input", function () {
        var listSearch = document.getElementById("listSearch");
        listSearch.style.display = "block";
        while (listSearch.firstChild){
            listSearch.removeChild(listSearch.firstChild);
        }

        var value = document.getElementById("search").value.trim();
        if (value.length == 0)
            return;

        var limit = Math.floor(value.length * 0.6 + 0.5);
        for (var i = 0; i < list.length; i++){
            var max = 0;
            for (var j = 0; j < list[i].length; j++){
                max = Math.max(max, LIC(list[i][j], value));
            }
            if (max >= limit){
                var li = document.createElement("li");
                li.innerText = list[i];
                li.onclick = function () {
                    document.getElementById("search").value = this.innerText;
                    document.getElementById("search").focus();
                    document.getElementById("listSearch").style.display = "none";
                };
                listSearch.appendChild(li);
            }
        }
    });

    $("#bsearch").on("click", function () {
        var value = document.getElementById("search").value.trim().replace(/[ \t-.]+/, " ");
        var pid = value.match(/(^\d+\s+)|(\s+\d+\s+)/);
        if (pid != null){
            for (var i = 0; i < pid.length; i++)
                if (pid[i] != null){
                    pid = pid[i];
                    break;
                }
        }
        var title = value.split(pid);
        pid = pid == null ? pid : pid.trim();
        if (title.length > 1){
            var tmp = "";
            for (var i = 0; i < title.length; i++)
                tmp += title[i] + " ";
            title = tmp.trim();
        }else {
            title = title[0];
        }
        title = title.trim().replace(/ +/g, "+");
        var urlParam = "own=1&info=long&" + (pid == "" ? "" : "pid=" + pid + "&") + "title=" + title;
        window.location.href = "/question/quickSearch?" + urlParam;
    });
});
