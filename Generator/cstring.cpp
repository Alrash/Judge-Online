/*************************************************************************
	> File Name: cstring.cpp
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Sat 23 Jul 2016 03:04:10 PM CST
 ************************************************************************/

#include <iostream>
#include <regex>
#include <set>
#include <algorithm>
#include "cstring.h"
#include "config.h"
#include "functions.h"
#ifdef _WIN32
#include <windows.h>
#endif

std::string trim(const std::string &str){
    if (str.empty()){
        return str;
    }

    std::string tmp = str;

    int pos;

    while ((pos = tmp.find_first_of("\t")) != std::string::npos){
        tmp.replace(pos, 1, " ");
    }

    tmp.erase(0, tmp.find_first_not_of(" "));
    tmp.erase(tmp.find_last_not_of(" ") + 1);
    
    return tmp;
}

std::vector<std::vector<std::string> > _regex_search(std::string str, const std::string &regex_expression){
    std::vector<std::vector<std::string> > vt;
    std::vector<std::string> tmp;
    std::smatch match;
    std::regex expression = std::regex(regex_expression);

    while(std::regex_search(str, match, expression)){
        for (auto item : match)
            tmp.push_back(item);
        vt.push_back(tmp);
        str = match.suffix().str();
        tmp.clear();
    }

    return vt;
}

CString::CString(){
    this->expression = "";
}

CString::CString(std::string expression){
    this->setExpression(expression);
}

/* *
 * argv[0]: 正则表达式
 * argv[1...]: 使用参数
 * 较于CString(string)，检测更准
 */
CString::CString(int argc, char *argv[]){
	this->expression = TRIM(argv[0]);

	std::vector<std::string> split_str;
    for (int i = 1; i < argc - 1; i++){
		if (argv[i][0] != '-'){
			std::cout << "忽略参数：" << argv[i] << std::endl;
		}else {
			split_str = split(argv[i] + 1, "=");
			if (split_str.size() == 2 && 
					used_parameter.find(split_str[0]) != used_parameter.end()){
				this->parameter.insert(std::pair<std::string, std::string>(split_str[0], split_str[1]));
			}else {
				std::cout << "忽略参数：" << argv[i] << std::endl;
			}
		}
	}
}

int CString::setExpression(const std::string &expression){
    this->expression = TRIM(expression);

    //获取参数对
	//检测使用参数是否存在，不存在出提示，存在从表达式中移除
	//本函数仅能检测-xx=xx的参数，及格式正确的参数，未能检测其余是否正确，如参数值
    for (auto item : _regex_search(this->expression, "(-)([^ \\t]+)(=)([^ \\t]+)")){
		if (used_parameter.find(*std::next(item.begin(), 2)) != used_parameter.end()){
			this->parameter.insert(std::pair<std::string, std::string>(*std::next(item.begin(), 2), *std::next(item.begin(), 4)));
			this->expression = this->expression.replace(this->expression.find(*(item.begin())), item.begin()->size(), "");
		}else {
			std::cout << "invalid parameter: " << *(item.begin()) << std::endl;
		}
    }

    //去除连续空格
    this->expression = TRIM(std::regex_replace(this->expression, std::regex(" {2,}"), " "));

    return 0;
}

/* *
 * 获取当前表达式
 */
std::string CString::getExpression() const{
    std::string result = this->expression;
    for (std::pair<std::string, std::string> item : this->parameter){
        result += " -";
        result += item.first;
        result += "=";
        result += item.second;
    }
    return result;
}

void CString::prompt(int pos, const std::string &errInfo, const std::string &which){
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);  
    WORD wOldColorAttrs;
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;  
      
    // Save the current color  
    GetConsoleScreenBufferInfo(h, &csbiInfo);  
    wOldColorAttrs = csbiInfo.wAttributes;  
    
    // Set the new color  
    SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_GREEN);  

    if (!this->expression.empty()){
    std::cerr << this->getExpression() << std::endl;
        for (int i = 0; i < pos; i++)
            std::cerr << " ";
        std::cerr << "^" << std::endl;
    }
    std::cerr << which <<  errInfo << std::endl;
    
    // Restore the original color  
    SetConsoleTextAttribute(h, wOldColorAttrs);
#else
    if (!this->expression.empty()){
        std::cerr << "\033[01;34m" << this->getExpression() << "\033[0m" << std::endl;
        for (int i = 0; i < pos; i++)
            std::cerr << " ";
        std::cerr << "\033[01;31m" << "^" << std::endl;
    }
    std::cerr << which << errInfo << "\033[0m" << std::endl;
#endif
}

#if _USE_FRIEND_ == 0
std::string CString::trim(const std::string &str){
    if (str.empty()){
        return str;
    }

    std::string tmp = str;

    tmp.erase(0, tmp.find_first_not_of(" "));
    tmp.erase(tmp.find_last_not_of(" ") + 1);
    
    return tmp;
}
#endif

/* *
 * before: -前一个字符
 * after: -后一个字符
 * start end: 范围
 */
bool CString::checkRange(char before, char after, char start, char end){
    //前一个括号，判断是否是start-end范围内的字符
    //后一个括号，判断after是否是在start-end范围内
    return (start <= before && before <= end) && (before <= after && after <= end);
}

CString::Info CString::roundBrackets(const std::string &str, int start){
	return Info();
}

/* *
 * 处理[，需要去重
 * 错误字符: '[' ' ' '{' '}' '(' ')'
 * info.str 记录解析出的字符串
 * info.step 记录到']'的长度
 */
CString::Info CString::squareBrackets(const std::string &str, int start){
    if (str.find(']', start) == std::string::npos){
        this->prompt(start, "没有发现']'");
        return Info(true);
    }

    Info info;
    info.step = 1;                          //]的一个
    for (int i = start + 1; str.at(i) != ']'; i++){
        switch(str.at(i)){
        case '[':
        case ' ':
        case '{':
        case '}':
        case '(':
        case ')':
            this->prompt(i, "多余字符");
            return Info(true);
        case '-':
            char before, after;
            before = str.at(i - 1);
            after = str.at(i + 1);

            if (!(this->checkRange(before, after, '0', '9')
                 || this->checkRange(before, after, 'a', 'z')
                 || this->checkRange(before, after, 'A', 'Z'))){
                this->prompt(i + 1, "范围不正确");
                return Info(true);
            }
            
            for (char ch = before; ch <= after; ch++){
                info.str += ch;
            }
            info.step += 2;
            i++;
            
            break;
        case '\\':
            if (binder.find(str.at(i + 1)) != binder.end()){
                info.str += binder.at(str.at(i + 1));
                info.step += 2;
                i++;
            }else {
                this->prompt(i + 1, "没有该字符的转义形式", "警告：");
                info.step++;
            }
            break;
        default:
            info.str += str.at(i);
            info.step++;
            break;
        }
    }

    //去重
    std::sort(info.str.begin(), info.str.end());
    info.str.resize(std::distance(info.str.begin(), std::unique(info.str.begin(), info.str.end())));

	return info;
}

/* *
 * 处理-
 * 主要是错误处理
 * 暂时无用 by 2016.07.28 22:53:00
 */
CString::Info CString::hyphen(const std::string &str, int start){
	return Info(true);
}

/* *
 * 处理转义字符\
 *     没有使用 by 2016.07.28 14:00:21
 */
CString::Info CString::escape(const std::string &str, int start){
    Info info;
    if (binder.find(str.at(start + 1)) != binder.end()){
        info.str += binder.at(str.at(start + 1));
        info.step += 2;
    }else {
        this->prompt(start + 1, "没有该字符的转义形式", "警告：");
        info.step++;
        info.isError = true;
    }
    return info;
}

/* *
 * 处理空格
 * 基本空格后面为错误待定参数
 * 含一个bug/考虑不周之处：若重复含有警告错误部分，每次仅指向第一个(find函数问题)
 */
CString::Info CString::blank(const std::string &str, int start){
    std::string substr = str.substr(start + 1);
    std::vector<std::string> left_parameter = split(substr, " ");

    //check错误
    for (auto& item : left_parameter){
        if (used_parameter.find(item.at(0) == '-' ? item.substr(1, item.find('=') - 1) : item.substr(0, item.find('='))) == used_parameter.end()){
            this->prompt(str.find(item), "没有该参数");
            return Info(true);
        }else {
            this->prompt(str.find(item), "该参数没有指定值，忽略", "警告：");
        }
    }
    return Info();
}

/* *
 * 外用接口，用于解析字符串
 * 有种想用指针，舍弃多余部分的想法（说的就是你info = ?(exp, i), if...）
 */
std::queue<std::vector<std::vector<Node> > > CString::parse(){
    if (this->expression.size() == 0){
        this->prompt(0, "表达式为空");
        return std::queue<std::vector<std::vector<Node> > >();
    }

    std::queue<std::vector<std::vector<Node> > > queue;
    std::vector<std::vector<Node> > item(1, std::vector<Node>(0));  //需要初始化
    Node node;
    Node::Description description;
    Info info;

    for (int i = 0; i < this->expression.size(); i++){
        switch(this->expression.at(i)){
        case '[':
            //std::cout << "[" << " " << i << std::endl;
            info = this->squareBrackets(this->expression, i);
            if (info.isError){
                return std::queue<std::vector<std::vector<Node> > >();
            }
            
            //增加描述信息
            description.start = node.str.size() - 1;
            node.str += info.str;
            description.end = node.str.size() - 1;
            node.description.push(description);
            
            //到达对应的']'字符位置
            i += info.step;
            break;
        case '|':
            //std::cout << "|" << " " << i << std::endl;
            //肯定错
            item.begin()->push_back(node);
            node = Node();
            break;
        case '{':
            std::cout << "{" << " " << i << std::endl;
            break;
        case '(':
               std::cout << "(" << " " << i << std::endl;
            break;
        case '\\':
            //std::cout << "\\" << " " << i << std::endl;
            if (binder.find(this->expression.at(i + 1)) != binder.end()){
                description.start = node.str.size() - 1;
                node.str += binder.at(this->expression.at(i + 1));
                description.end = node.str.size() - 1;
                node.description.push(description);
                i++;
            }else {
                this->prompt(i + 1, "没有该字符的转义形式", "警告：");
            }
            break;
        case ' ':
            //std::cout << " " << " " << i << std::endl;
            info = this->blank(this->expression, i);
            if (info.isError){
                return std::queue<std::vector<std::vector<Node> > >();
            }
            break;
        case '-':   //想使用，请使用转义字符'\'
        case ')':
        case '}':
        case ']':
            this->prompt(i + 1, "多余字符");
            return std::queue<std::vector<std::vector<Node> > >();
        default:
            std::cout << "default " << i << " " << this->expression.at(i) << std::endl;
            description.start = node.str.size() - 1;
            node.str += this->expression.at(i);
            description.end = node.str.size() - 1;
            node.description.push(description);
            break;
        }
    }

    return queue;
}
