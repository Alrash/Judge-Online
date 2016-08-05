/*************************************************************************
  > File Name: generator.cpp
  > Author: Alrash
  > Mail: kasukuikawai@gmail.com
  > Created Time: Tue 02 Aug 2016 02:44:06 AM CST
 ************************************************************************/

#include <ctime>
#include <random>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <regex>
#include "generator.h"
#include "cstring.h"
#include "functions.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

Generator::Generator(){
    deque = std::deque<Unit>();
    count = 0;

    for (int i = 0; i < VALUE_LINE + 1; i++)
        for (int j = 0; j < VALUE_COLUMN + 1; j++)
            use_value[i][j] = 0;

    //设置目录名
    //格式：mon-day_time(NULL)
    time_t now;
    tm *info;
    time(&now);
    info = localtime(&now);
    dir = std::to_string(info->tm_mon + 1) + "-" + std::to_string(info->tm_mday) + "_" + std::to_string(now);

    //创建文件夹
#ifdef _WIN32
    _mkdir(dir.c_str());
    dir += "\\";
#else
    //mkdir(dir.c_str(), 0755);
    dir = "test";
    dir += "/";
#endif
};

int Generator::setUnit(std::queue<std::vector<Columns> > queue, std::map<std::string, std::string> parameter){
    if (queue.empty() || parameter.empty()){
        return -1;
    }

    deque.push_back(Unit(queue, parameter));
    return 0;
}

int Generator::setCount(int count){
    this->count = count;
    return 0;
}

void Generator::resetDeque(){
    int pos = 0;
    std::vector<std::vector<std::string> > search;
    for (auto item : this->deque){
        if (item.parameter[RECT].compare(default_parameter.at(RECT))){
            //与默认值不同，赋值第..个，清空原本的
            item.parameter[RECT] = replaceAll(item.parameter[RECT], " ", "");
            std::next(this->deque.begin(), std::stoi(item.parameter[RECT]) - 1)->parameter[RECT] = item.parameter[RECT];
            item.parameter[RECT] = default_parameter.at(RECT);
        }

        //更新use_value表
        search = _regex_search(item.parameter[NUMCOLUMN], "l(\\d+)c(\\d+)");
        if (!search.empty()){
            use_value[std::stoi(search[0][1])][std::stoi(search[0][2])] = 1;
        }
        search = _regex_search(item.parameter[NUMLINE], "l(\\d+)c(\\d+)");
        if (!search.empty()){
            use_value[std::stoi(search[0][1])][std::stoi(search[0][2])] = 1;
        }
        search = _regex_search(item.parameter[RECT], "(\\d+,){2}(l(\\d+)c(\\d+))");
        if (!search.empty()){
            use_value[std::stoi(*std::prev(search[0].end(), 2))][std::stoi(*std::prev(search[0].end(), 1))] = 1;
        }
    }
}

int Generator::fileAppend(char *filename, std::string lineContent){
    FILE *fp;

    if ((fp = fopen(filename, "a")) == NULL){
        std::cerr << "无法打开" << filename << "文件" << std::endl;
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%s\n", lineContent.c_str());
    fclose(fp);

    return 0;
}

int Generator::rand(int start, int end){
    if (start == end)
        return start;

    if (start > end)
        std::swap(start, end);

    std::random_device rd;
    return (rd() % (end + 1 - start)) + start;
}

std::vector<std::string> Generator::splitRect(std::string rect){}

/* *
 * 作用：用于生成文件序列
 * 正确反0，错误反-1
 */
int Generator::generator(){
    if (deque.empty()){
        std::cerr << "队列为空" << std::endl;
        return -1;
    }
    if (!count){
        std::cerr << "没有设置生成次数，默认采用1" << std::endl;
        this->count = 1;
    }

    char filename[256];
    int numcolumn = 0, numline = 0, step = 0, rectCount = 0;
    std::queue<std::vector<Columns> > queue;
    std::map<std::string, std::string> parameter;
    auto search = _regex_search("test", "\\w+");
    std::string lineContent = "";

    //输出文件次数
    for (int times = 0; times < this->count; times++){
        sprintf(filename, "%sinput%.2d.txt", dir.c_str(), times + 1);
        lineContent = "";
        //单次生成
        for (auto line = deque.begin(); line != deque.end(); line++){
            queue = line->queue;
            parameter = line->parameter;

            //区域循环次数
            //部分赋值
            if (std::regex_match(parameter[RECT], std::regex("(\\d+,){2}(\\d+)"))){
                search = _regex_search(parameter[RECT], "(\\d+,){2}(\\d+)");
                rectCount = std::stoi(*std::prev(search[0].end(), 1));
                step = std::stoi(search[0][1]);
            }else {
                search = _regex_search(parameter[RECT], "(\\d+,){2}(l(\\d+)c(\\d+))");
                rectCount = this->use_value[std::stoi(*std::prev(search[0].end(), 2))][std::stoi(*std::prev(search[0].end(), 1))];
                step = std::stoi(search[0][1]);
            }
            for (int numRect = 0; numRect < rectCount; numRect++){
                //行区域生成
                for (auto subline = line; subline != std::next(line, step); subline++){
                    //行循环次数
                    search = _regex_search(subline->parameter[NUMLINE], "l(\\d+)c(\\d+)");
                    if (search.empty()){
                        numline = std::stoi(subline->parameter[NUMLINE]);
                    }else {
                        numline = this->use_value[std::stoi(search[0][1])][std::stoi(search[0][2])];
                    }
                    for (int numLine = 0; numLine < numline; numLine++){
                        //行表达式，单行循环次数
                        search = _regex_search(subline->parameter[NUMCOLUMN], "l(\\d+)c(\\d+)");
                        if (search.empty()){
                            numcolumn = std::stoi(subline->parameter[NUMCOLUMN]);
                        }else {
                            numcolumn = this->use_value[std::stoi(search[0][1])][std::stoi(search[0][2])];
                        }
                        for (int numColumn = numcolumn; numColumn < numcolumn; numcolumn++){
                            //真正的循环queue得到单行样例信息
                            lineContent += "";
                        }
                    }
                }
                //修正偏移量
                line = std::next(line, step - 1);
            }
        }
    }
}

int Generator::generator(int count){
    this->setCount(count);
    this->generator();
}
