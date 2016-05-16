<?php
/**
 * Author: Alrash
 */

$level = array(100, 200, 500, 1000, 2000, 4000, 8000, 12000, 20000);

function halfSearch($search, $num_array)
{
    $len = count($num_array);
    $low = 0;
    $high = $len - 1;

    //检测是否为数值数组，否，返回false
    if (!is_array($num_array))
        return false;
    foreach ($num_array as $value)
        if (!is_numeric($value))
            return false;

    while ($low <= $high)
    {
        $mid = floor(($low + $high) / 2);
        if ($num_array[$mid] > $search)
            $high = $mid - 1;
        elseif ($num_array[$mid] < $search)
            $low = $mid + 1;
        else
            return $mid;
    }

    return -1;
}

function getExp($exp)
{
    $level = array(100, 200, 500, 1000, 2000, 4000, 8000, 12000, 20000);
    //global $level;
    $top = getLevel($exp);
    
    return $exp . '/' . $level[$top];
}

function getLevel($exp)
{
    $level = array(100, 200, 500, 1000, 2000, 4000, 8000, 12000, 20000);
    foreach ($level as $key => $value)
        if ($exp < $value)
            return $key;
    return 10;
}
