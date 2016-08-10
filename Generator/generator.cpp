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
    mkdir(dir.c_str(), 0755);
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

std::string Generator::getDir() const{
    return this->dir;
}

void Generator::resetDeque(){
    int pos = 0;
    std::vector<std::vector<std::string> > search;
    for (auto item : this->deque){
        //更新use_value表
        search = _regex_search(item.parameter[NUMCOLUMN], "l(\\d+)c(\\d+)");
        if (!search.empty()){
            use_value[std::stoi(search[0][1])][std::stoi(search[0][2])] = 1;
        }
        search = _regex_search(item.parameter[NUMLINE], "l(\\d+)c(\\d+)");
        if (!search.empty()){
            use_value[std::stoi(search[0][1])][std::stoi(search[0][2])] = 1;
        }
        search = _regex_search(item.parameter[RECTANGLE], "(\\d+,){2}(l(\\d+)c(\\d+))");
        if (!search.empty()){
            use_value[std::stoi(*std::prev(search[0].end(), 2))][std::stoi(*std::prev(search[0].end(), 1))] = 1;
        }

        if (item.parameter[RECTANGLE].compare(default_parameter.at(RECTANGLE))){
            //与默认值不同，赋值第..个，清空原本的
            item.parameter[RECTANGLE] = replaceAll(item.parameter[RECTANGLE], " ", "");
            std::next(this->deque.begin(), std::stoi(item.parameter[RECTANGLE]) - 1)->parameter[RECTANGLE] = item.parameter[RECTANGLE];
            item.parameter[RECTANGLE] = default_parameter.at(RECTANGLE);
        }

    }
}

int Generator::fileAppend(const char *filename, std::string lineContent, std::string mode){
    FILE *fp;

    if ((fp = fopen(filename, mode.c_str())) == NULL){
        std::cerr << "无法打开" << filename << "文件" << std::endl;
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%s\n", lineContent.c_str());
    fclose(fp);

    return 0;
}

/* *
 * 返回start至end中的随机值
 * 闭区间[start, end]
 */
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
    this->resetDeque();

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
            if (std::regex_match(parameter[RECTANGLE], std::regex("(\\d+,){2}(\\d+)"))){
                search = _regex_search(parameter[RECTANGLE], "(\\d+,){2}(\\d+)");
                rectCount = std::stoi(*std::prev(search[0].end(), 1));
                step = std::stoi(search[0][1]);
            }else {
                search = _regex_search(parameter[RECTANGLE], "(\\d+,){2}(l(\\d+)c(\\d+))");
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
                        //行生成
                        lineContent = this->getLineContent(subline->queue, numcolumn);
                        //更新map值(表，非key->value)
                        this->updateUse_Value(lineContent, std::distance(deque.begin(), subline) + 1);
                        this->fileAppend(filename, lineContent);
                    }
                }
            }
            //修正偏移量
            line = std::next(line, step - 1);
        }
    }
}

int Generator::generator(int count){
    this->setCount(count);
    this->generator();
}

std::string Generator::getLineContent(const std::queue<std::vector<Columns> > &queue, int times){
    std::string lineContent = "";
    std::queue<std::vector<Columns> > copy;
    std::queue<Columns::Node::Description> queue_description;
    std::vector<Columns> line;
    Columns::Node node;
    Columns::Node::Description description;
    int pos, count = 0, size = 0;

    //行循环次数
    for (int i = 0; i < times; i++){
        copy = queue;
        //真正的循环queue得到单行样例信息
        while(!copy.empty()){
            line = copy.front();
            pos = this->rand(0, line[0].vNode.size() - 1);
            node = line[0].vNode[pos];
            size = rand(line[0].description.min_times, line[0].description.max_times);

            for (int m = 0; m < size; m++){
                //使用描述信息生成字符串
                queue_description = node.description;
                while(!queue_description.empty()){
                    description = queue_description.front();
                    count = rand(description.min_times, description.max_times);
                    
                    for (int n = 0; n < count; n++){
                        if (std::regex_match(node.str.substr(description.start, description.end + 1 - description.start), std::regex(secondRoundBrackets))){
                            //匹配到二层括号
                            int linenum = std::stoi(replaceAll(node.str.substr(description.start, description.end + 1 - description.start), "#", ""));
                            lineContent += this->getSecondRoundBrackets(line[linenum]);
                        }else {
                            lineContent += node.str.at(this->rand(description.start, description.end));
                        }
                    }
                    
                    queue_description.pop();
                }
            }
            copy.pop();
        }
        lineContent += " ";
    }
    return trim(lineContent);
}

std::string Generator::getSecondRoundBrackets(Columns subline){
    std::string str = "";
    Columns::Node::Description description;
    std::queue<Columns::Node::Description> queue_description;
    Columns::Node node = subline.vNode[rand(0, subline.vNode.size() - 1)];
    int times, count = rand(subline.description.min_times, subline.description.max_times);

    for (int i = 0; i < count; i++){
        queue_description = node.description;
        while (!queue_description.empty()){
            description = queue_description.front();
            times = rand(description.min_times, description.max_times);
            for (int i = 0; i < times; i++)
                str += node.str[rand(description.start, description.end)];
            queue_description.pop();
        }
    }

    return str;
}

void Generator::updateUse_Value(std::string lineContent, int line){
    std::vector<std::string> num = split(lineContent, " ");

    for (int i = 1; i < VALUE_COLUMN + 1; i++)
        if (this->use_value[line][i] && num.size() >= i){
            this->use_value[line][i] = std::stoi(num[i - 1]);
        }

#if __TEST__ != 0
    for (int i = 0; i < 6; i++){
        for (int j = 0; j < 10; j++)
            std::cout << this->use_value[i][j] << " ";
        std::cout << std::endl;
    }
#endif
}
