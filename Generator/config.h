/*************************************************************************
	> File Name: config.h
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Tue 26 Jul 2016 12:51:15 PM CST
 ************************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H

#include <set>
#include <string>

/* *
 * 还没有具体定下来
 * norepeat: 不重复，使用$0,$1... rect
 * numcolumn: 列数
 * numline: 行数
 */
const std::set<std::string> used_parameter = {
	"norepeat",
	"numcolumn",
	"numline"
};

#endif
