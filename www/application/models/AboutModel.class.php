<?php
/**
 * Author: Alrash
 * 关于页面相关
 * 本模型对于建议的存储采用xml文件的形式保存，原因:懒
 * 本模型对于xml文件的操作采用simpleXML方式，原因:懒
 * 由于部分原因：所有对于xml文件的操作均未加锁
 * xml文件存储路径/www/DATA/aboutSuggest.xml
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

class AboutModel extends Model
{
    protected $xml;

    function __construct()
    {
        parent::__construct();
        $this->xml = simplexml_load_file(DATA_PATH . 'aboutSuggest.xml');
    }

    function saveXml()
    {
        // TODO: Implement __destruct() method.
        $this->xml->asXML(DATA_PATH . 'aboutSuggest.xml');
    }
    
    function getXmlElement()
    {
        return $this->xml;
    }

    function addSuggest($suggest = array())
    {
        $no = (int)$this->xml->total + 1;
        $floor = $this->xml->addChild('floor');
        $floor->addChild('No', $no);
        $floor->addChild('id', $suggest['id']);
        $floor->addChild('suggest', $suggest['suggest']);
        $this->xml->total = $no;
        $this->saveXml();
    }
    
    function getIdInfo($column = array(), $table, $where)
    {
        return $this->selectArraySpecialIndex($this->createSelectSql($column, $table, $where), 'UId');
    }
}