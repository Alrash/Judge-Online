<?php
/**
 * Author: Alrash
 * 本控制用于java课程设计使用
 */

class MessageBoradController extends AboutController
{
	//接受java端发送的信息
	function message()
	{
		$operation = new AboutModel;
		$message = isset($_GET['message']) ? $_GET['message'] : $_POST['message'];
		$id = isset($_SESSION['UId']) ? $_SESSION['UId'] : 0;
		$operation->addSuggest(array('suggest' => $message, 'id' => $id));
	}

	//调出xml文件内容，转换成html文件的格式显示至getMessage页面上
	//需要htmlspeicalchars转换字符
	function getMessage()
	{
		$operation = new AboutModel;

        //获得UId值作为索引的数组，便于下面的函数替换
        $needData = $operation->selectReturnSpecialIndex(array('`UId`', '`Nickname`', '`Exp`'),
                        '`User_View`', $this->getSuggestSelectWhere($operation->getXmlElement()));
        $data = $this->dealSuggestArray($operation->getXmlElement(), $needData);
		
		//转换成html的格式
		$content = "";
		foreach ($data as $floorNo => $item)
		{
			$content .= "<html>";
			$content .= "Author:" . $item['Nickname'] . '<br>';
			$content .= 'Content:<br>' . $item['suggest'];
			$content .= "</html>@@@";
		}
		$content = htmlspecialchars(substr($content, 0, strlen($content) - 3));
		$this->set('data', $content);
	}

	//重载析构函数，直接输出结果，不附加其余东西
	function __destruct()
	{
		$this->_view->showDealResult();
	}
}
