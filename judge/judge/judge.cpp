/*************************************************************************
	> File Name: judge.cpp
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Wed 31 Aug 2016 10:49:59 AM CST
    > Function: add info to pipe, in order to be raed by judged program
    > Notice: 本程序含有至少两个不足之处
                1. 所有的错误信息没有写入配置文件中的log下的error中
                   原因是，在judged中自动生成的日志文件使用root:root(0755)权限，
                    本函数的调用一般是php文件，其用户为一般为http并且不可能存
                    在在root组中，导致无法写入
                   解决方案：自己创建.conf中log组下的文件，并且更改用户
                    (sudo chown judge:judge ?? && gpasswd -a judge http之类),
                    所有fprintf(stderr, "***", ...)之类可换成
                        sprintf(error_message, "***", ...);
                        Log log = Log("error.log");
                        log.append(error_message);
                2. 当管道文件无法写入时，没有记录信息以便下次使用或者检查数据库，
                    搜集未检测的sid，及不太智能
 ************************************************************************/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <map>
#include <regex>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include "../public/readConfig.h"
#include "../public/functions.h"
#include "../public/config.h"

#ifndef __TEST__
#define __TEST__ 0
#endif

#if __TEST__ != 0
#include <iostream>
using namespace std;
#endif

#define PATH_LENGTH 1024

/* *
 * parameter 包含在public/config.h中
 * content 需要传输的内容，顺序，又parameter定
 */
std::vector<std::string> compiler;
std::map<std::string, std::string> parameter_map = {
    {"-t", "1000"},
    {"-m", "256"},
};
char pipe_fifo[PATH_LENGTH];
std::string content = std::string();
const int open_mode = O_WRONLY | O_NONBLOCK;

void showHelp(){
    printf("参数使用说明:\n\
    * -s: sid \\d+ 提交号\n\
    * -p: pid \\d+ 问题号\n\
    * -u: uid \\d+ 用户id号\n\
    * -l: language c/c++/c++\\d\\d/java/python\\d.\\d 提交语言(使用配置文件)\n\
    * -t: \\d+(ms) time 运行时间 单位ms 可省 默认1000ms\n\
    * -m: \\d+(MB) memory 运行内存 单位MB 可省 默认256MB\n\
    * -c: \\d+ count 测试文件数\n\
    * -h: help 输出帮助文档\n");
}

int init(int argc, char* argv[]){
    ReadConfig config;

    /* *
     * 获取编译选项
     * 防止重复编译
     */
    compiler = config.getGroupKeys("compiler");
    if (compiler.empty()){
        fprintf(stderr, "cannot find compiler group in config file\n");
        return -1;
    }

    /* *
     * 检测参数
     * 由main上面的参数介绍可知，除了"-c"参数，其余均为数字
     */
    for (int i = 1; i < argc; i++){
        if (is_find_in_vector(parameter, argv[i])){
            /* *
             * 输出帮助文档
             */
            if (std::string("-h").compare(argv[i]) == 0){
                showHelp();
                exit(0);
            }

            /* *
             * 获取参数值
             * 防止越界
             */
            ++i;
            if (i == argc){
                fprintf(stderr, "cannot find value of parameter %s\n", argv[i - 1]);
                return -1;
            }

            if (std::string("-l").compare(argv[i - 1])){
                //the other parameter
                if (is_numeric(argv[i])){
                    parameter_map[argv[i - 1]] = argv[i];
                }else {
                    fprintf(stderr, "find wrong value %s with parameter %s, need number\n", argv[i], argv[i - 1]);
                    return -1;
                }
            }else {
                //equal to "-l"
                if (is_find_in_vector(compiler, argv[i])){
                    parameter_map["-l"] = argv[i];
                }else {
                    fprintf(stderr, "cannot find value %s with parameter %s\n", argv[i], argv[i - 1]);
                    return -1;
                }
            }
        }else {
            fprintf(stderr, "parameter %s is not existed\n", argv[i]);
            return -1;
        }
    }

    //函数参数-h，所以需要减1
    if (parameter_map.size() != (parameter.size() - 1)){
        fprintf(stderr, "参数个数不对，放弃这次程序使用\n");
        return -1;
    }

    /* *
     * 整合信息，编合成PIPE_LENGTH长度的信息串
     * 具体格式为：参数1 对应值;参数2 对应值;....@@@@
     *          ;由splitch代替 @由fillch代替
     */
    for (auto item : parameter){
        if (item.compare("-h")){
            content += std::string(item + " " + parameter_map[item]) + splitch;
        }
    }
    content.resize(PIPE_LENGTH, fillch);

    /* *
     * 获取pipe路径，检测是否可以写入
     * 这里没有做万一不能写入的处理，仅仅为输出提示信息
     */
    strncpy(pipe_fifo, config.getKeyValue("pipe", "path").c_str(), PATH_LENGTH - 1);
    if (access(pipe_fifo, W_OK) == -1){
        char error_message[1024];
        sprintf(error_message, "cannot write pipe file %s", pipe_fifo);
        perror(error_message);
        return -1;
    }

    return 0;
}

/* *
 * -s: sid \d+ 提交号
 * -p: pid \d+ 问题号
 * -u: uid \d+ 用户id号
 * -l: language c/c++/c++11/java/python3.5 提交语言(使用配置文件)
 * -t: \d+(ms) time 运行时间 单位ms
 * -m: \d+(MB) memory 运行内存 单位MB
 * -c: \d+ count 测试文件数
 * -h: help 输出参数说明
 */
int main(int argc, char* argv[]){
    if (init(argc, argv) != 0){
        return -1;
    }
    
    int pipeid = -1;
    //保证能够打开管道文件
    while(true){
        if ((pipeid = open(pipe_fifo, open_mode)) != -1){
            //打开管道文件
            if (write(pipeid, content.c_str(), PIPE_LENGTH) == 1){
                fprintf(stderr, "write error\n");
                exit(EXIT_FAILURE);
            }
            close(pipeid);
            break;
        }
    }

    return 0;
}
