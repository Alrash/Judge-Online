/*************************************************************************
	> File Name: check.h
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Sun 17 Jul 2016 08:55:00 PM CST
 ************************************************************************/

/* *
 * 返回值约束：
 *     本文件内函数检测比较笼统，返回值仅含有三个
 *     -1：文件打开错误
 *     1：输出结果错误
 *     0：结果完全正确
 */

#ifndef _CHECK_H
#define _CHECK_H

/* *
 * 检查输出结果
 */
int checkAnswer(const char* outfile, const char* output);

#endif
