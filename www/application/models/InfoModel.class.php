<?php
/**
 * 个人信息模型
 */

class InfoModel extends Model
{
    function checkNickname($nickname)
    {
        $response = $this->select(array("Nickname"), "UserInfo",  "`Nickname` = " . addQuotes($nickname));
        return $this->dealCheckResponse($response);
    }
    
    function checkEmail($email)
    {
        $response = $this->select(array("Email"), "UserInfo",  "`Email` = " . addQuotes($email));
        return $this->dealCheckResponse($response);
    }
    
    /**
     * 用于处理check函数从模型获得的数据
     * 返回结果:don't get     数据库查询错误(Fail)
     *         ok            没有查询的值(null)，说明未被使用/注册
     *         fail          查询到值(有值),说明已被使用
     */
    final private function dealCheckResponse($response)
    {
        if (!is_null($response) && $response == "Fail")
            return "don't get";
        else
            if (is_null($response))
                return "ok";
            else
                return "fail";
    }
    
    function addNewUser($columnvalue = array())
    {
        return $this->insert('UserInfo', $columnvalue);
    }
    
    function getValues($column = array(), $table, $where)
    {
        return $this->select($column, $table, $where);
    }
}