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

#define _USE_FRIEND_ 1

/* *
 * 替换parse函数返回值中的string
 */
struct Node{
	struct Description{
		int start, end;
		int max_times, min_times;

		Description(){
			min_times = max_times = 1;
			start = end = 0;
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

/* *
 * 替换所有的tab键为单空格，删除首尾所有空格
 */
std::string trim(const std::string &str);
/* *
 * 使用正则表达式切离函数
 */
std::vector<std::vector<std::string> > _regex_search(std::string str, const std::string &regex_expression);

/* *
 */
const static std::map<char, std::string> binder = {
	{'d', "0123456789"},
	{'w', "abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ"},
	{' ', " "},
	{'[', "["},
	{']', "]"},
	{'{', "{"},
	{'}', "}"},
	{'\\', "\\"},
	{'-', "-"}
};

class CString{
public:
	CString();
	CString(std::string expression);
	CString(int argc, char *argv[]);

	int setExpression(const std::string &expression);
	std::string getExpression() const;
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
	
	std::queue<std::vector<std::vector<Node> > > parse();
private:
	std::string expression;
	//std::vector<std::vector<std::string> > parameter;
    std::map<std::string, std::string> parameter;

	//解析函数返回值结构体
	struct Info{
		int step;
		bool isError;
		std::string str;

		Info(bool isError = false){
			this->isError = isError;
			this->step = 0;
			this->str = std::string("");
		};

		Info& operator=(const Info &info){
			this->isError = info.isError;
			this->step = info.step;
			this->str = info.str;
			return *this;
		};
	};

	//检查-范围是否正确
	bool checkRange(char before, char after, char start, char end);

    //处理字符
    Info roundBrackets(const std::string &str, int start);		//'('
    Info squareBrackets(const std::string &str, int start);		//'['
    Info hyphen(const std::string &str, int start);				//'-'
	Info escape(const std::string &str, int start);				//'\'
	Info blank(const std::string &str, int start);				//' '

};

#endif
