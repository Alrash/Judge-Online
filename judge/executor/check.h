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
 *     0：输出结果错误
 *     1：结果完全正确
 */

#ifndef _CHECK_H
#define _CHECK_H

/* *
 * 检查每一个输出结果
 */
int checkEachAnswer(int num);

/* *
 * 检查每一行输出结果
 * 注意：严格检查每一行输出结果，所以近支持linux的换行符
 */
int checkLineAnswer(int num);

#endif
