/*************************************************************************
	> File Name: config.h
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Thu 14 Jul 2016 10:53:39 PM CST
 ************************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H

#include <string>

//带编译文件的根目录
//暂时手动设置绝对路径
//Submit目录下设置内容：check -- 测试文件夹
//                      d*（提交号）-- 存放提交文件，以及一些编译输出文件（如：警告、错误提示、分值结果）
//const std::string root = "/www/DATA/Submit";
const std::string root = "/home/alrash/Desktop/judge";
const int MB = 1024 * 1024;						//1M
const int max_outfile_size = MB;				//1M

//测试结果常量字符串
const int ANSWER_AC = 0;
const int ANSWER_WA = 1;
const int ANSWER_PE = 2;
const int ANSWER_RE = 3;
const int ANSWER_TLE = 4;
const int ANSWER_MLE = 5;
const int ANSWER_OLE = 6;
const int ANSWER_CE = 7;
const int ANSWER_OTHERS = 100;

#endif
