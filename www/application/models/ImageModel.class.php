<?php
/**
 */
class ImageModel extends Model{
    public function getRightWrongSize($pid) {
        $data = $this->selectArray($this->createSelectSql(array('Per', 'Total'), 'Question_View', "PId = $pid"));
        $total = $data[0]['Total'] == 0 ? 1: $data[0]['Total'];
        $right = $data[0]['Per'] == null ? 0 : round($data[0]['Per'] * $total / 100);
        return array($right, $total - $right);
    }

    public function getCompiler($pid){
        $condition = "sum(if(compiler = 'c', 1, 0)) as 'c', sum(if(compiler = 'c++', 1, 0)) as 'c++',
            sum(if(compiler = 'c++11', 1, 0)) as 'c++11', sum(if(compiler = 'java', 1, 0)) as 'java',
            sum(if(compiler = 'python3.5', 1, 0)) as 'python3.5'";
        $data = $this->selectArray("select $condition from Submission_View where PId = $pid");
        return array($data[0]['c'], $data[0]['c++'], $data[0]['c++11'], $data[0]['java'], $data[0]['python3.5']);
    }
}