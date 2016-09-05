/**
 * Author: Alrash
 * create time: 2016-08-18
 */

var list;
var pageFontAndBack = {};
var param = "";

function clearTable(table) {
    var child = table.getElementsByTagName("tr");

    for (var i = 0; child.length != 0 && child.length != 1; i++){
        if (child[i].className != "columnTitle"){
            child[i].remove();
            i--;
        }
    }
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

function sendAndResponse(page) {
    $.ajax({
        async: true,
        type: "get",
        url: "/info/getInfoWithJson",
        data: "mode=question&info=long&page=" + page + param,
        dataType: "json",
        error: function () {
            alert("network connected error");
        },
        success:function (response) {
            list = eval(response);
            var table = document.getElementById("tableList");
            var className = Array("cell01", "cell02");
            clearTable(table);
            for (var i = 0; i < list.length - 1; i++){
                fillTable(list[i], className[i % 2], table);
            }
            setPageMenu(list[list.length - 1].now, list[list.length - 1].total);
        },
    });
}

function setPageMenu(now, pagesize) {
    var pageUL = document.getElementById("pageUL");
    var pageMenu = pageUL.getElementsByTagName("li");

    while(pageMenu.length){
        if (pageMenu[0].id == "font" || pageMenu[0].id == "back"){
            pageFontAndBack[pageMenu[0].id] = pageMenu[0].innerText;
        }
        pageMenu[0].remove();
    }
    while(pageUL.getElementsByTagName("span").length){
        pageUL.getElementsByTagName("span")[0].remove();
    }

    var li = document.createElement("li");
    //innerText href ... is not constructor, so can not use (new document.createElement("??")).xxx()
    var a = document.createElement("a");

    //font button
    a.setAttribute("href", "javascript:void(0);");
    a.id = 'font';
    a.innerText = pageFontAndBack['font'];
    a.onclick = function () {
        sendAndResponse(1);
    };
    li.appendChild(a);
    pageUL.appendChild(li);
    a = document.createElement("a");

    //中间构造
    var start = 1, end = 8;
    if (now < 5){
        end = Math.min(end, pagesize) + 1;
    }else if (pagesize - now > 3){
        end = now + 4;
        start = now - 4;    //now - 5 + 1
    }else {
        end = pagesize + 1;
        start = Math.max(end - 8, 1);
    }

    for (var i = start; i < end; i++){
        if (i != now){
            li = document.createElement("li");
            a = document.createElement("a");
            a.innerText = i;
            a.setAttribute("href", "javascript:void(0);");
            li.appendChild(a);
            (function(value){
                a.addEventListener("click", function() {
                    sendAndResponse(value);
                }, false);})(i);
            pageUL.appendChild(li);
        }else{
            var span = document.createElement("span");
            span.innerText = i;
            pageUL.appendChild(span);
        }
    }

    //back button
    a = document.createElement("a");
    a.setAttribute("href", "javascript:void(0);");
    a.id = 'back';
    a.innerText = pageFontAndBack['back'];
    a.onclick = function () {
        sendAndResponse(pagesize);
    }
    li = document.createElement("li");
    li.appendChild(a);
    pageUL.appendChild(li);
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


function fillTable(obj, className, table){
    var tr = document.createElement("tr");
    var debug = document.createElement("td");
    var title = document.createElement("td");
    var type = document.createElement("td");
    var count = document.createElement("td");
    var per = document.createElement("td");
    var hard = document.createElement("td");
    var a = document.createElement("a");
    var aTitle = document.createElement("a");
    var background = document.createElement("div");
    var right = document.createElement("div");
    var showText = document.createElement("div");
    var percent = (obj.Per != null ? (obj.Per + "%") : "0.00%")

    tr.className = className;
    debug.className = "debug";
    title.className = "title";
    type.className = "type";
    count.className = "count";
    per.className = "per";
    hard.className = "hard";

    a.setAttribute("href", "/question/debug/" + obj.PId);
    a.appendChild(document.createElement("i"));
    debug.appendChild(a);
    aTitle.setAttribute("href", "/question/pid/" + obj.PId);
    aTitle.setAttribute("target", "_blank");
    aTitle.innerText = obj.PId + " - " + obj.Title;
    title.appendChild(aTitle);
    type.innerText = obj.Type;
    count.innerHTML = obj.Total;
    background.className = "backgroundTable";
    right.className = "right";
    right.style.width = percent;
    right.innerText = "...";
    showText.className = "showText";
    showText.innerText = percent;
    background.appendChild(right);
    background.appendChild(showText);
    per.appendChild(background);
    hard.className = "hard";
    hard.innerText = obj.Hard;

    tr.appendChild(debug);
    tr.appendChild(title);
    tr.appendChild(type);
    tr.appendChild(count);
    tr.appendChild(per);
    tr.appendChild(hard);

    table.appendChild(tr);
}

function allChecked(value) {
    var checkbox = document.getElementsByTagName("input");
    for (var i = 0; i < checkbox.length; i++){
        if (checkbox[i].type == "checkbox"){
            if (value || checkbox[i].value < 100)
                checkbox[i].checked = value;
        }
    }
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
                //alert("error");
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

    $('[name=select]').on('click', function () {
        allChecked(true);
    });

    $("[name='clear']").on('click', function () {
        allChecked(false);
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

        var checkbox = document.getElementsByClassName("checkboxIn");
        var hard = "";
        var question_type = "", isExist = false;
        if (checkbox != null){
            for (var i = 0; i < checkbox.length; i++)
                if (checkbox[i].checked && checkbox[i].value < 100 && checkbox[i].value > 0){
                    hard += checkbox[i].value + ";";
                }else if (checkbox[i].checked && checkbox[i].value >= 100){
                    if (!isExist){
                        question_type = checkbox[i].value - 100;
                        isExist = true;
                    }else {
                        question_type = "";
                    }
                }
        }

        param = "&" + (pid == null ? "" : ("pid=" + pid + "&")) + (title == "" ? "" : ("title=" + title + "&"))
            + (hard == "" ? "" : ("hard=" + hard + "&")) + (question_type == "" ? "" : "type=" + question_type);

        $.ajax({
            async: true,
            type: "get",
            url: "/info/getInfoWithJson",
            data:"mode=question&info=long" + param,
            dataType: "json",
            error:function () {
                alert("network connected error");
            },
            success:function (response) {
                list = eval(response);
                var table = document.getElementById("tableList");
                var className = Array("cell01", "cell02");
                clearTable(table);
                for (var i = 0; i < list.length - 1; i++){
                    fillTable(list[i], className[i % 2], table);
                }
                setPageMenu(list[list.length - 1].now, list[list.length - 1].total);
            }
        });
    });

    $("#skip").on("keypress", function () {
        if (event.keyCode != 13)
            return;
        var page = document.getElementById("skip").value.trim();
        if (page.match(/^[1-9]\d*$/)){
            sendAndResponse(page);
            document.getElementById("skip").value = "";
            document.getElementById("skip").blur();
        }
    });
});

window.onload = function () {
    allChecked(true);

    $(function () {
        (function ($) {
            $.getUrlParam = function (name) {
                var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)");
                var r = window.location.search.substr(1).match(reg);
                if (r != null) return unescape(r[2]); return null;
            }
        })(jQuery);

        var own = $.getUrlParam('own');
        var pid = $.getUrlParam("pid");
        var title = $.getUrlParam("title");
        var long = $.getUrlParam('info');

        if (own == 1){
            param = "&" + (pid == null ? "" : ("pid=" + pid + "&")) + (title == null ? "" :("title=" + title));
        }

        var inputText = document.getElementById("search");
        if (inputText != null)
            inputText.value = (pid == null ? "" : (pid + " - ")) + (title == null ? "" : (title.indexOf('+') == -1 ? title : title.replace(/\+/g, " ")));

        $.ajax({
            async: true,
            type: "get",
            url: "/info/getInfoWithJson",
            data: "mode=question&info=long" + param,
            dataType: "json",
            error:function () {
                alert("network connected error");
            },
            success:function (response) {
                list = eval(response);
                var table = document.getElementById("tableList");
                var className = Array("cell01", "cell02");
                clearTable(table);
                for (var i = 0; i < list.length - 1; i++){
                    fillTable(list[i], className[i % 2], table);
                }
                setPageMenu(list[list.length - 1].now, list[list.length - 1].total);
            }
        });
    });
}