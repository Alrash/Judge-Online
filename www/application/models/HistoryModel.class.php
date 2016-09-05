<?php

/**
 * Author: Alrash
 * Date: 2016/08/29 19:08
 * Description:
 */
class HistoryModel extends Model{
    public function getAllSubmissions($uid, $page){
        $start = RECORD_SIZE * ($page - 1);
        $end = RECORD_SIZE * $page;
        return $this->selectArray("select PId, UId, SId, Status, Title from Submission_View where UId = $uid limit $start, $end");
    }

    public function getSubmissionNumber($uid){
        return $this->selectArray("select count(*) as size from Submission_View where UId = $uid")[0]['size'];
    }

    public function getQuestionInfo($sid){
        $pid = $this->selectArray("select PId, compiler, goal from Submission_View where SId = $sid");
        if (is_null($pid))
            return null;

        $ini['pid'] = $pid[0]['PId'];
        $path = DATA_PATH . "Questions/" . $ini['pid'] . "/detail.ini";

        if (file_exists($path)){
            $data = parse_ini_file($path);

            $ini['compiler'] = $pid[0]['compiler'];
            $ini['title'] = $data['questionTitle'];
            $ini['testFile'] = $data['testFile'];               //测试数据数量.count目测是total
            $ini['type'] = $data['type'];

            return $ini;
        }
        return null;
    }

    public function getSubmissionContent($sid, $pid, $type, $compiler){
        $path = DATA_PATH . "Submit/$sid/";
        switch ($compiler){
            case 'c': $extension = 'c'; break;
            case 'c++': $extension = 'cpp'; break;
            case 'c++11': $extension = 'cpp'; break;
            case 'java': $extension = 'java'; break;
            case 'python': $extension = 'py'; break;
            default: $extension = 'c'; break;
        }
        $content = null;
        if ($type == 0){
            $contentPath = $path . "$sid.$extension";
            if (file_exists($contentPath) && ($file = fopen($contentPath, "r"))){
                $content = array();
                while (!feof($file)){
                    array_push($content, htmlspecialchars(fgets($file)));
                }
                fclose($file);
            }
        }else {
            $sourcePath = DATA_PATH . "Questions/$pid/" . "source.$extension";
            $answerPath = $path . "answer.txt";
            if (file_exists($sourcePath) && file_exists($answerPath)
                && ($answer = fopen($answerPath, "r"))
                && ($source = fopen($sourcePath, "r"))){
                $content = array();
                $content['source'] = array();
                $content['answer'] = array();

                while (!feof($answer)){
                    array_push($content['answer'], htmlspecialchars(fgets($answer)));
                }

                while (!feof($source)){
                    array_push($content['source'], htmlspecialchars(fgets($source)));
                }

                fclose($answer);
                fclose($source);
            }
        }

        return $content;
    }
}