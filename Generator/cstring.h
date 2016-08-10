/*************************************************************************
  > File Name: cstring.h
  > Author: Alrash
  > Mail: kasukuikawai@gmail.com
  > Created Time: Sat 23 Jul 2016 03:08:32 PM CST
  > 所有int返回值：负值表示错误，0表示正确，正值见函数说明
 ************************************************************************/

#ifndef _CSTRING_H
#define _CSTRING_H

#include <string>
#include <queue>
#include <vector>
#include <map>
#include "config.h"

#define _USE_FRIEND_ 1

/* *
 * 替换所有的tab键为单空格，删除首尾所有空格
 */
std::string trim(const std::string &str);
/* *
 * 使用正则表达式切离函数
 */
std::vector<std::vector<std::string> > _regex_search(std::string str, const std::string &regex_expression);

/* *
 * 转义字符键值对
 *   key: 待转义字符
 *   value: 替换后的字符串
 */
const static std::map<char, std::string> binder = {
    {'d', "0123456789"},
    {'w', "abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ"},
    {' ', " "},
    {'[', "["},
    {']', "]"},
    {'{', "{"},
    {'}', "}"},
    {')', ")"},
    {'\\', "\\"},
    {'-', "-"},
    {'.', "."}
};

/* *
 * 1) 解析表达式所用类
 *    原本是想在string上扩展，现为独立类，用于生成待处理字符串单元
 *    可将config.h中__TEST__置为非0值，查看信息单元整体情况
 * 2) 下次改造时，可以换成其他类名
 * 3) 对外接口(具体情况参见实现注释)
 *    CString() CString(string) CString(int, char*)
 *    int setExpression(string)                     //设置表达式的值，准备解析
 *    string getExpression()                        //获取当前表达式
 *    map<string, string> getParameter()            //获取参数键值对
 *    vector<string> _regex_search(string, string)  //正则匹配搜索
 *    queue<vector<Columns>>parse()                 //解析函数
 *    void prompt(int, string, string)              //打印错误信息
 */
class CString{
public:
    CString();
    CString(std::string expression);
    CString(int argc, char *argv[]);

    int setExpression(const std::string &expression);
    std::string getExpression() const;
    int setParameter();
    std::map<std::string, std::string> getParameter() const;
    friend std::vector<std::vector<std::string> > _regex_search(std::string str, const std::string &regex_expression);
    void prompt(int pos, const std::string &errInfo, const std::string &which = "错误：");

#if _USE_FRIEND_ == 0
    //暂时仅能除去blank
    std::string trim(const std::string &str);
#define TRIM this->trim
#else
    friend std::string trim(const std::string &str);
#define TRIM trim
#endif

    std::queue<std::vector<Columns> > parse();
private:
    std::string expression;
    std::map<std::string, std::string> parameter;

    //解析函数返回值结构体
    struct Info{
        int step;
        bool isError;
        Columns::Node::Description description;
        std::string str;

        Info(bool isError = false){
            this->isError = isError;
            this->step = 0;
            this->str = std::string("");
            this->description = Columns::Node::Description();
        };

        Info& operator=(const Info &info){
            this->isError = info.isError;
            this->step = info.step;
            this->str = info.str;
            this->description = info.description;
            return *this;
        };
    };

    //检查-范围是否正确
    bool checkRange(char before, char after, char start, char end);
    bool checkParameter(const std::string &parameter, std::string &value);

    //处理字符
    //为什么部分不合时宜地加了s？ 处理一对而已
    Info roundBrackets(const std::string &str, int start);         //'('
    Info squareBrackets(const std::string &str, int start);        //'['
    Info braces(const std::string &str, int start);                //'{'
    Info hyphen(const std::string &str, int start);                //'-'
    Info escape(const std::string &str, int start);                //'\'
    Info blank(const std::string &str, int start);                 //' '

};

#endif
