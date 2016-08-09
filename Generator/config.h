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
#include <string>

/* *
 * 还没有具体定下来
 * norepeat: 不重复，使用$-1(y轴方向), $0(行内), $1(第1个括号内)... rect(line * column)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 已使用的 by 2016.08.03 01:33:55
 * numcolumn: 列数，具体数字或lXcY(x行y列所对应的数值)
 * numline: 行数，同上
 * rect:重复区域，{x, n, lXcY / num}(第x行开始的n行，重复几次；例如{1,2,2}，包含第1、2两行，并且这两行需重复两次)
 */
#define NUMCOLUMN "numcolumn"
#define NUMLINE   "numline"
#define RECTANGLE "rectangle"
const std::set<std::string> used_parameter = {
    //"norepeat",
    NUMCOLUMN,
    NUMLINE,
    RECTANGLE
};
const static std::map<std::string, std::string> default_parameter = {
    {NUMCOLUMN, "1"},
    {NUMLINE, "1"},
    {RECTANGLE, "0,1,1"}
};

//第二层括号替换符，line表示第几行
const std::string REPLACE = "\\d+";
const std::string secondRoundBrackets = "##" + REPLACE +"##";

//0表示不是测试 1测试
#define __TEST__ 0

/* *
 * 最初的计划: vector<vector<string>>
 * Columns: 替换vector<string>
 * Node: 替换string
 * 现变量: vector<Columns>
 */
struct Columns{
    struct Node{
        struct Description{
            int start, end;
            int max_times, min_times;
    
            Description(){
                min_times = max_times = 1;
                start = end = 0;
            };

            Description(const Description &description){
                *this = description;
            };
    
            Description& operator=(const Description &description){
                this->start = description.start;
                this->end = description.end;
                this->min_times = description.min_times;
                this->max_times = description.max_times;
                return *this;
            };
        };
    
        std::string str;
        std::queue<Description> description;
    
        Node(){
            str = std::string("");
            description = std::queue<Description>();
        };
    
        Node(const Node &node){
            *this = node;
        }
    
        Node& operator=(const Node &node){
            this->str = node.str;
            this->description = node.description;
            return *this;
        };
    };

    std::vector<Node> vNode;
    Node::Description description;                //暂时使用min_times和max_times

    Columns(){
        vNode = std::vector<Node>(0, Node());
        description = Node::Description();
    };

    Columns(const Columns &Columns){
        *this = Columns;
    };

    Columns& operator=(const Columns &Columns){
        this->vNode = Columns.vNode;
        this->description = Columns.description;
        return *this;
    };
};

#endif
