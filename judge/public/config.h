/*************************************************************************
	> File Name: config.h
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Thu 14 Jul 2016 10:53:39 PM CST
 ************************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H

#include <vector>
#include <map>
#include <string>

/* *
 * judge使用参数，具体信息参见judge.cpp main函数注释
 */
const std::vector<std::string> parameter = {
    "-s", "-p", "-u", "-l", "-c", "-t", "-m", "-h"
};
//管道中固定字串长度
const int PIPE_LENGTH = 512;
//管道中用于填充与切割的字符
const char fillch = '@';
const char splitch = ';';

const int COMPILER_ERROR = 1;
const int INIT_ERROR = 11;
const int MB = 1024 * 1024;
const int max_outfile_size = MB;				//1M

//测试结果常量字符串
const int TESTING = 0;
const int ANSWER_AC = 1;
const int ANSWER_WA = 2;
const int ANSWER_PE = 3;
const int ANSWER_RE = 4;
const int ANSWER_TLE = 5;
const int ANSWER_MLE = 6;
const int ANSWER_OLE = 7;
const int ANSWER_CE = 8;
const int ANSWER_OTHERS = 9;

const std::map<std::string, std::string> answer_map = {
    {std::to_string(TESTING), "testing"},
    {std::to_string(ANSWER_AC), "AC"},
    {std::to_string(ANSWER_WA), "WA"},
    {std::to_string(ANSWER_PE), "PE"},
    {std::to_string(ANSWER_RE), "RE"},
    {std::to_string(ANSWER_TLE), "TLE"},
    {std::to_string(ANSWER_MLE), "MLE"},
    {std::to_string(ANSWER_OLE), "OLE"},
    {std::to_string(ANSWER_CE), "CE"},
    {std::to_string(ANSWER_OTHERS), "others"},
};

//数据库使用
const char HOST[] = "127.0.0.1";
const char USERNAME[] = "JudgeOnline";
const char PASSWORD[] = "judgement";
const char DATABASE[] = "JudgeOnline";
const int PORT = 3306;

#endif
