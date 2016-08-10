/*************************************************************************
    > File Name: generator.h
    > Author: Alrash
    > Mail: kasukuikawai@gmail.com
    > Created Time: Tue 02 Aug 2016 02:44:06 AM CST
 ************************************************************************/

#ifndef _GENERATOR_H
#define _GENERATOR_H

#include <string>
#include <map>
#include <vector>
#include <deque>
#include <queue>
#include "config.h"

#define VALUE_LINE 100
#define VALUE_COLUMN 100

class Generator{
public:
    Generator();

    int setUnit(std::queue<std::vector<Columns> > queue, std::map<std::string, std::string> parameter);
    int setCount(int count);
    std::string getDir() const;

    int generator();
    int generator(int count);
    int fileAppend(const char *filename, std::string lineContent, std::string mode = std::string("a"));

private:
    struct Unit{
        std::queue<std::vector<Columns> > queue;
        std::map<std::string, std::string> parameter;

        Unit(){
            this->queue = std::queue<std::vector<Columns>>();
            this->parameter = std::map<std::string, std::string>();
        }

        Unit(const Unit &unit){
            *this = unit;
        }

        Unit(std::queue<std::vector<Columns> > queue, std::map<std::string, std::string> parameter){
            this->queue = queue;
            this->parameter = parameter;
        }

        Unit& operator=(const Unit &unit){
            this->queue = unit.queue;
            this->parameter = unit.parameter;
            return *this;
        }
    };

    std::deque<Unit> deque;         //信息单元(双向队列)
    std::string dir;                //文件夹名
    int count;                      //生成文件数目
    //记录LxCy值，默认为0，其余表示有用
    int use_value[VALUE_LINE + 1][VALUE_COLUMN + 1];

    void resetDeque();              //更改队列内部分内容
    int rand(int start, int end);
    std::vector<std::string> splitRect(std::string rect);
    std::string getLineContent(const std::queue<std::vector<Columns> > &queue, int times);
    void updateUse_Value(std::string lineContent, int line);
    std::string getSecondRoundBrackets(Columns subline);
};

#endif
