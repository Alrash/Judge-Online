/**
 * Author: Alrash
 * function: 用于问题页面的动态css处理
 */

/* *
 * 获取c_name的cookie值
 */
function getCookie(c_name)
{
    if (document.cookie.length>0)
    {
        var c_start=document.cookie.indexOf(c_name + "=")
        if (c_start!=-1)
        {
            c_start=c_start + c_name.length+1
            var c_end=document.cookie.indexOf(";",c_start)
            if (c_end==-1) c_end=document.cookie.length
            return unescape(document.cookie.substring(c_start,c_end))
        }
    }
    return "";
}

function isLogin(){
    var login = getCookie("SignIn");
    if (login != null && login != "1"){
        window.location.href = "/login/signIn";
    }
}

$(document).ready(function(){
    $('.open-box').on('click',function(){
        $('.overlay,.submission').fadeIn(200);
    });

    $('.overlay,.close span').on('click',function(){
        $('.overlay,.submission').fadeOut(200,function(){
            $(this).removeAttr('style');
        });
    });

    //提交问题
    $('#buttonSubmit').on('click', function () {
        var numTag = document.getElementById("sPid").getElementsByTagName("input");
        var isQuick = false;
        var error = document.getElementsByClassName("error");
        var regex = new RegExp("^[1-9]\\d*$");
        var num = 0;

        error[0].innerText = "";

        //检测并获取题号
        if (numTag.length == 1){
            isQuick = true;
            if (!regex.test(numTag[0].value.trim())){
                error[0].innerText = "请输入数字";
                return;
            }

            numTag[0].value = numTag[0].value.trim();
            num = numTag[0].value;
        }else if(numTag.length == 0){
            var pidTag = document.getElementById("sPid");
            if (!/^#[1-9]\d*$/.test(pidTag.innerText)){
                error[0].innerText = "请勿修改该标签";
                return;
            }

            num = pidTag.innerText.substr(1);
        }else {
            return;
        }

        var compilerTag = document.getElementById("tab").getElementsByTagName("li");
        var compiler = "";

        for (var i = 0; i < compilerTag.length; i++){
            if (compilerTag[i].className == "current"){
                switch (compilerTag[i].innerText){
                    case "C": compiler = "c"; break;
                    case "C++": compiler = "c++"; break;
                    case "C++11": compiler = "c++11"; break;
                    case "Java": compiler = "java"; break;
                    case "Python3.5": compiler = "python"; break;
                    default: compiler = "fail"; break;
                }
                break;
            }
        }

        if (compiler == "fail"){
            return;
        }

        var contentTag = document.getElementsByClassName("code");
        if (contentTag.length != 1){
            return;
        }

        var type = contentTag[0].getElementsByTagName("textarea");
        var split = "#@@#"
        var content = "";
        if (type.length == 1){
            //综合题
            content = type[0].value;
        }else{
            //填空题
            var blank = contentTag[0].getElementsByTagName("input");
            var isError = false;
            for (var i = 0; i < blank.length; i++){
                if (blank[i].value.trim() != ""){
                    content += blank[i].value.trim() + split;
                }else{
                    blank[i].value = "";
                    blank[i].placeholder = "请输入答案";
                    blank[i].className = "inputError";
                    isError = true;
                }
            }
            if (isError)
                return;

            content = content.substr(0, content.length - split.length);
        }

        //异步发送信息
        $.ajax({
            async: true,
            type: "post",
            url: "/info/submission",
            data:{
                quick: isQuick,
                pid: num,
                compiler: compiler,
                code: content,
            },
            error:function () {
                alert("网络错误");
            },
            success:function (response) {
                if (response == "fail"){
                    alert("请确认题号是否正确");
                }else if (response == "ok"){
                    alert("提交成功");
                    //地址转向
                    //和本弹窗一样，弹窗+跳转(提交面弹窗+计时+提前确认键)

                    //弹窗偷懒 by 2016/08/29
                    alert("提交成功");
                    window.location.href = "/history/user";
                }else if (response == "existed"){
                    alert("所提交的编程语言不在题目包含之列");
                }else if (response == "logout"){
                    alert("没有登录");
                    window.location.href="/login/signIn";
                }else{
                    alert("不明错误\n" + response + "\n请联系管理员");
                }
            },
        });
    });

    $(".code input").on('keyup', function () {
        $(this).removeAttr("class");
    });
});

window.onload = function () {
    isLogin();

    var oLi = document.getElementById("tab").getElementsByTagName("li");
    var oDiv = document.getElementById("content").getElementsByTagName("div");

    oLi[0].setAttribute("class", "current");

    for(var i = 0; i < oLi.length; i++) {
        oLi[i].index = i;
        oLi[i].onclick = function () {
            for(var n = 0; n < oLi.length; n++) oLi[n].className="";
            this.className = "current";
            for(var n = 0; n < oDiv.length; n++) oDiv[n].className = "hide";
            oDiv[this.index].className = "show"
        }
    }
}
