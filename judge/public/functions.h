/*************************************************************************
	> File Name: functions.h
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Mon 11 Jul 2016 10:12:39 PM CST
    > Function: 提供一些共用功能函数
 ************************************************************************/

#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

#include <vector>
#include <string>

/* *
 * 切割字符串函数
 * @param:
 *      str -- 待切割字符串
 *      pattern -- 分割的字符串
 * @return:
 *      返回字符串向量组
 */
std::vector<std::string> split(const std::string &str, const std::string &pattern);

#endif
