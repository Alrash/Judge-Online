<?php
/**
 * Author: Alrash
 * 关于页面相关的东西
 * xml文件格式:
 *  <Suggests>
 *      <total>N</total>                //当前已记录数，只增不减
 *      <floor>                         //楼层
 *          <No>"#n"</No>               //当前楼层号
 *          <id>num</id>                //提出者id号，游客为-1
 *          <suggest>"str"</suggest>    //具体建议(已被转义)
 *      </floor>
 *  </Suggests>
 */
require_once LIB_PATH . 'String.php';

class AboutController extends Controller
{
    function __construct($controller, $action)
    {
        parent::__construct($controller, $action);
        $this->set('Title', 'help');
        $this->set('bodyStyle', 'help');
    }

    function index()
    {//暂无，直接输出
    }

    function help()
    {
    }

    protected function getSuggestSelectWhere($xml)
    {
        //得到floor数组,处理后得到非重复的用户id值
        $floor = $xml->floor;
        $id = array();
        foreach ($floor as $key => $item)
        {
            $item = (array)$item;
            array_push($id, (int)$item['id']);
        }
        $id = array_unique($id);    //id去重

        //使用$id拼接得到$where条件
        $where = '';
        foreach ($id as $value)
            $where .= '`UId` = ' . $value . ' or ';
        return substr_replace($where, '' , strlen($where) - 4);
    }

    /**
     * 将xml文件与userInfo数组中的内容合并
     * 最后数组格式：
     *        "floor No." => array(Nickname, Image, Exp, Trust, suggest),
     *         ...
     */
    protected function dealSuggestArray($xml, $userInfo = array())
    {
        $floor = $xml->floor;
        $info = array();
        //设置游客情况
        array_push($userInfo, array('0' => array('UId' => '0', 'Nickname' => 'visitor',
						'Image' => '/img/default_image.jpg', 'Exp' => 0, 'Trust' => 0)));
        
        //组合字符串
		foreach ($floor as $Key => $item)
        {
            $item = (array)$item;
            $no = (int)$item['No'];
            $id = (int)$item['id'];
            //转换suggest中的特殊字符
            $info[$no]['suggest'] = htmlBlank(htmlReturn(modifyStringItem((string)$item['suggest'])));
            foreach ($userInfo[$id] as $key => $value)
                $info[$no][$key] = $value;
            unset($info[$no]['UId']);
        }

		return $info;
	}

    function suggest()
    {
        $operation = new AboutModel;

        //接受提交的信息增加至xml文件中
        //如果为游客提交，则id为0
        if(isset($_POST['suggest']))
        {
            $suggest = $_POST['suggest'];
            $id = isset($_SESSION['UId']) ? $_SESSION['UId'] : 0 ;
            $operation->addSuggest(array('suggest' => $suggest, 'id' => $id));
        }

        //获得UId值作为索引的数组，便于下面的函数替换
        $needData = $operation->selectReturnSpecialIndex(array('`UId`', '`Nickname`', '`Image`', '`Exp`', '`Trust`'),
                        '`User_View`', $this->getSuggestSelectWhere($operation->getXmlElement()));
        $this->set('data', $this->dealSuggestArray($operation->getXmlElement(), $needData));
    }

    function license()
    {
    }

    function __destruct()
    {
        // TODO: Implement __destruct() method.
        $this->_view->specialRenderAbout();
    }
}