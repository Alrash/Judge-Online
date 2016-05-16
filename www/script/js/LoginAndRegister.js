/**
 * Author: Alrash
 * 用于显示、处理登录和注册时的信息
 * 因为使用了load刷新token值，以至于多次与页面交互( >﹏< )
 */

//检查是否含有. ? @ \ " !等字符
function checkString(string) {
    if (string.indexOf(".") != -1 || string.indexOf("?") != -1 || string.indexOf("@") != -1
        || string.indexOf("\\") != -1 || string.indexOf("\"") != -1 || string.indexOf("!") != -1
        || string.indexOf(">") != -1 || string.indexOf("=") != -1)
        return false;
    return true;
}

//计算字符长度
function getStringLength(string) {
    var len = 0;
    
    for (var i = 0; i < string.length; i++) {
        if (string[i].match(/[^x00-xff]/ig) != null)
            len += 2;
        else
            len += 1;
    };
    
    return len;
}

//检查字符串长度
function limitStringLength(string, left, right) {
    var len = getStringLength(string);
    if (len < left || len > right)
        return false;
    return true;
}

//获得加密key
//额。。。已由session代替
function getKey() {
}

//对password进行对称加密
//使用默认aes加密，请保证key和iv为16或32位字符串
function getCryptoGraph(string, key, iv) {
    var iv = CryptoJS.enc.Utf8.parse(iv);
    return CryptoJS.AES.encrypt(string, CryptoJS.enc.Utf8.parse(key), { iv: iv,mode:CryptoJS.mode.CBC,padding:CryptoJS.pad.ZeroPadding});
}

$(document).ready(function() {
    var isNicknameOK, isEmailOk, isPasswdOK, isRepasswdOK;
    
    //检查nickname是否符合不含不应含有的字符，以及长度是否在6-20之间
    //符合返回true, 否则false
    $("#nickname_reg").blur(function() {
        isNicknameOK = false;
        
        //注册时，失去昵称焦点，异步请求数据库，查看是否已被注册
        //但是，首先应对字符串进行处理
        var nickname = $("#nickname_reg").val().trim();
        $("#nickname_reg").css("border", "solid 1px red");
        if (!limitStringLength(nickname, 4, 20)) {
            document.getElementById("NickErr").innerHTML = "昵称字符长度应在4-20个之间";
            return;
        }
        else
            document.getElementById("NickErr").innerHTML = "";
            
        if (!checkString(nickname)) {
            document.getElementById("NickErr").innerHTML = "昵称中不能含有以下字符 = ？>  \\ \" @ ! .";
            return;
        }
        else
            document.getElementById("NickErr").innerHTML = "";

        //get请求，查看昵称是否已被注册
        $.ajax({
            async:true,
            type:"get",
            url:"/info/checkNickname",
            data:"nickname=" + nickname,
            error:function () {
                $("#NickErr").css("font-size", "1.2em");
                document.getElementById("NickErr").innerHTML = "网络连接错误，请检查网络";
            },
            success:function (response) {
                if (response == "fail")
                    document.getElementById("NickErr").innerHTML = "该昵称已被注册";
                if (response == "ok")
                {
                    $("#nickname_reg").css("border", "solid 1px #339933");
                    isNicknameOK = true;
                }
                if (response == "don't get")
                    document.getElementById("NickErr").innerHTML = "数据库连接错误，请联系管理员";
            }
        });
        $("#refresh").load("/log/register #hidden", null);

        //设置输入文本，取出首尾空格
        $("#nickname_reg").val(nickname);
    });

    /**
     * 邮件检测
     * 首先使用正则，检测格式的正确性
     * 再检测是否被注册
     */
    $("#email_reg").blur(function () {
        isEmailOk = false;
        
        var email = $("#email_reg").val().trim();
        $("#email_reg").val(email);
        $("#email_reg").css("border", "solid 1px red");
        $("#EmailErr").css("font-size", "0.8em");
        document.getElementById("EmailErr").innerHTML = "";

        //查看邮箱是否正确
        var regex = new RegExp(/\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/);
        if (!regex.test(email)) {
            document.getElementById("EmailErr").innerHTML = "邮箱格式不正确";
            return;
        }

        //get提交email_reg中的值，查看邮箱是否被注册
        $.ajax({
            async:true,
            type: "get",
            url: "/info/checkEmail",
            data: "email=" + email,
            error:function () {
                $("#EmailErr").css("font-size", "1.2em");
                document.getElementById("EmailErr").innerHTML = "网络连接错误，请检查网络";
            },
            success:function (response) {
                if (response == "fail")
                    document.getElementById("EmailErr").innerHTML = "该邮箱已被注册";
                if (response == "ok")
                {
                    $("#email_reg").css("border", "solid 1px #339933");
                    isEmailOk = true;
                }
                if (response == "don't get")
                    document.getElementById("NickErr").innerHTML = "数据库连接错误，请联系管理员";
            }
        });
        $("#refresh").load("/log/register #hidden", null);
    });
    
    //防止有一部分空格的情况
    $("#passwd_reg").blur(function () {
        isPasswdOK = false;
        
        var passwd = $("#passwd_reg").val().trim();
        $("#passwd_reg").val(passwd);
        document.getElementById("PasswdErr").innerHTML = "";
        
        if (!limitStringLength(passwd, 6, 20)) {
            document.getElementById("PasswdErr").innerHTML = "密码长度需在6~20个字符之间";
            $("#passwd_reg").css("border", "solid 1px red");
            return;
        }
        
        $("#passwd_reg").css("border", "solid 1px #339933");
        isPasswdOK = true;
    });
    
    //无用
    $("#repasswd_reg").blur(function () {
        //if ($("#passwd_reg").val() != $("#repasswd_reg").val())
    });
    
    //防止注册时出现问题，提示信息变化
    $("#nickname_reg").focus(function () {
        $("#Error").css("font-size", "0.8em");
        $("#Error").css("color", "red");
    });

    //监听键盘事件
    //Login页面使用
    $(":input").bind("keyup change", function () {
        if (document.getElementById("Error")) {
            $("#Error").css("font-size", "0.8em");
            $("#Error").css("color", "red");
            document.getElementById("Error").innerHTML = "";
        }
    });
    $(":input").bind("blur", function () {
       if (document.getElementById("Error")) {
           //get方式，获得隐藏span域的值
           $.ajax({
               async: true,
               type: "get",
               url: "/info/randomNumber",
               data: "n=" + Math.random(),
               error:function () {
                   document.getElementById("Error").innerHTML = "网络连接错误，请重试";
               }
           });

           //刷新隐藏域，获得真正的值
           $("#refresh").load("/log/signIn #hidden", null);
       }
    });

    $("#nickname_reg").keyup(function () {
        var nickname = $("#nickname_reg").val();
        if (!limitStringLength(nickname, 4, 20)) {
            document.getElementById("NickErr").innerHTML = "昵称字符长度应在4-20个之间";
            return;
        }
        else
            document.getElementById("NickErr").innerHTML = "";

        if (!checkString(nickname)) {
            document.getElementById("NickErr").innerHTML = "昵称中不能含有以下字符 = ？>  \\ \" @ ! .";
            return;
        }
        else
            document.getElementById("NickErr").innerHTML = "";
        
        $("#nickname_reg").css("border", "solid 1px #339933");
    });

    $("#passwd_reg").keyup(function () {
        var passwd = $("#passwd_reg").val().trim();
        document.getElementById("PasswdErr").innerHTML = "";

        if (!limitStringLength(passwd, 6, 20)) {
            document.getElementById("PasswdErr").innerHTML = "密码长度需在6~20个字符之间";
            $("#passwd_reg").css("border", "solid 1px red");
            return;
        }
        $("#passwd_reg").css("border", "solid 1px #339933");
    });

    $("#repasswd_reg").keyup(function () {
        isRepasswdOK = false;
        document.getElementById("RepasswdErr").innerHTML = "";
        
        /**
         * $("#passwd_reg").val() == ""
         * 防止未填passwd，直接点击repasswd_reg输入框，获得isRepasswdOK true值
         */
        if ($("#passwd_reg").val() == "" || $("#passwd_reg").val() != $("#repasswd_reg").val()){
            document.getElementById("RepasswdErr").innerHTML = "输入密码不一致";
            $("#repasswd_reg").css("border", "solid 1px red");
            return;
        }
        
        $("#repasswd_reg").css("border", "solid 1px #339933");
        isRepasswdOK = true;
    });
    
    /************使用超链接提交表单*************/
    //提交注册信息
    $("#submit").click(function () {
        if(isEmailOk && isNicknameOK && isPasswdOK && isRepasswdOK)
        {
            var passwd = $("#passwd_reg").val();
            var nickname = $("#nickname_reg").val();
            var email = $("#email_reg").val();
            var token = $("#hidden").html();
            
            passwd = getCryptoGraph(passwd, token, token);
            /**
             * 使用post方式提交注册信息
             * 至于为什么在passwd后加 "
             * 因为使用默认aes加密后，passwd字串末尾为==，报maximum call stack size exceeded错误
             * 不太熟悉http协议和jquery，这里猜测是jquery截断字符串出错
             * */
            $.ajax({
                async: true,
                type: "post",
                url: "/info/addNewUser",
                data: {
                    nickname:nickname,
                    email:email,
                    passwd:passwd + '"'
                    //passwd:passwd
                },
                beforeSend:function () {
                    $("#NickErr").css("font-size", "1.2em");
                    $("#NickErr").css("color", "khaki");
                    document.getElementById("NickErr").innerHTML = "注册中...";
                },
                error:function () {
                    $("#NickErr").css("font-size", "1.2em");
                    document.getElementById("NickErr").innerHTML = "网络连接错误，请检查网络";
                },
                success:function (response) {
                    if (response == "fail")
                        document.getElementById("NickErr").innerHTML = "注册失败，请重试";
                    else
                    {
                        //去除error和beforeSend出现的提示信息
                        document.getElementById("NickErr").innerHTML = "";

                        //隐藏注册栏，显示提示信息
                        $(".area").hide();
                        $(".area_reg_sub").hide();
                        //别问我，为什么这么暴力，别的方法都试了，成功的只想用这个  囧rz
                        $(".success").css("visibility", "visible");
                        
                        //暂停两秒，跳转至首页
                        setTimeout("window.location.href='/index'", 2000);
                    }
                }
            });
        }
    });
    
    //提交登录
    //NameId  想不出名字而起的名字
    $("#login").click(function () {
        //取出需要的值，去除首尾空格
        var uid = $("#NameId").val().trim();
        var passwd = $("#PassWd").val().trim();
        var isRemember = $("input:checked").is(":checked") ? 1 : 0;
        var token = $("#hidden").html();

        $("#NameId").val(uid);
        $("#PassWd").val(passwd);

        if (getStringLength(uid) < 4) {
            document.getElementById("Error").innerHTML = "用户名应大于四个字符"
            return;
        }
        if (getStringLength(passwd) < 6) {
            document.getElementById("Error").innerHTML = "应输入6位以上的密码";
            return;
        }

        //加密密码
        passwd = getCryptoGraph(passwd, token, token);
        document.getElementById("Error").innerHTML = token;
        
        $.ajax({
            async: true,
            type: "post",
            url: "/info/signIn",
            data:{
                nickname:uid,
                passwd:passwd + '"',
                remember:isRemember
            },
            beforeSend:function () {
                $("#Error").css("font-size", "1.2em");
                $("#Error").css("color", "khaki");
                document.getElementById("Error").innerHTML = "登录中...";
            },
            error:function () {
                $("#Error").css("font-size", "1.2em");
                $("#Error").css("color", "red");
                document.getElementById("Error").innerHTML = "网络连接错误，请检查网络";
            },
            success:function (response) {
                $("#Error").css("font-size", "0.8em");
                $("#Error").css("color", "red");
                if (response == "fail")
                    document.getElementById("Error").innerHTML = "用户名或密码错误";
                if (response == "ok")
                {
                    document.getElementById("Error").innerHTML = "";
                    //跳转至首页
                    setTimeout("window.location.href='/index'", 500);
                }
            }
        });
    });
});