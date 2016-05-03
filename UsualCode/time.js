/**
 * Created by lovelive on 4/8/16.
 */
function clock()
{
    var today = new Date();
    var hour = today.getHours();
    var min = today.getMinutes();
    var sec = today.getSeconds();

    //in order to get type like "01"
    hour = modifyTimeItem(hour);
    min = modifyTimeItem(min);
    sec = modifyTimeItem(sec);

    document.getElementById("Time").innerHTML = hour + ":" + min + ":" + sec;
    t = setTimeout('clock()', 500);
}

function modifyTimeItem(time)
{
    if (time < 10)
        return "0" + time;
    else
        return time;
}