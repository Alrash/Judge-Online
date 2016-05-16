/**
 * Author: Alrash
 * 用于菜单上的样式控制
 */
$(document).ready(function () {
    $("#img").hover(function () {
       document.getElementById("img").setAttribute("id", "img_over");
       document.getElementById("usual").setAttribute("id", "over");
    }, function () {
        document.getElementById("img_over").setAttribute("id", "img");
        document.getElementById("over").setAttribute("id", "usual");
    });
});
