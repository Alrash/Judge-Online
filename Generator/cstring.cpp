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
    this->setParameter();
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
    this->setParameter();

    std::vector<std::string> split_str;
    for (int i = 1; i < argc - 1; i++){
        if (argv[i][0] != '-'){
            this->prompt(0, argv[i], "忽略参数：");
        }else {
            split_str = split(argv[i] + 1, "=");
            if (split_str.size() == 2 && 
                    used_parameter.find(split_str[0]) != used_parameter.end()){
                if (checkParameter(split_str[0], split_str[1])){
                    this->parameter[split_str[0]] = split_str[1];
                }else {
                    this->prompt(0, std::string(argv[i]) + std::string("参数格式错误"), "忽略参数：");
                }
            }else {
                this->prompt(0, argv[i], "忽略参数：");
            }
        }
    }
}

int CString::setExpression(const std::string &expression){
    this->expression = TRIM(expression);
    this->setParameter();

    //获取参数对
    //检测使用参数是否存在，不存在出提示，存在从表达式中移除
    //本函数仅能检测-xx=xx的参数，及格式正确的参数，未能检测其余是否正确，如参数值
    for (auto item : _regex_search(this->expression, "(-)([^ \\t]+)(=)([^ \\t]+)")){
        if (used_parameter.find(*std::next(item.begin(), 2)) != used_parameter.end()){
            if (this->checkParameter(*std::next(item.begin(), 2), *std::next(item.begin(), 4))){
                this->parameter[*std::next(item.begin(), 2)] = *(std::next(item.begin(), 4));
                this->expression = this->expression.replace(this->expression.find(*(item.begin())), item.begin()->size(), "");
            }else{
                this->prompt(0, std::string(*(item.begin())) + std::string("参数格式错误"), "invalid parameter");
            }
        }else {
            this->prompt(0, *(item.begin()), "invalid parameter");
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
        if (!item.first.compare(RECTANGLE)){
            result += "{";
            result += item.second;
            result += "}";
	}else
            result += item.second;
    }
    return result;
}

int CString::setParameter(){
    for (auto item : used_parameter){
        this->parameter[item] = default_parameter.at(item);
    }
    return 0;
}

/* *
 * 获取额外参数对
 */
std::map<std::string, std::string> CString::getParameter() const{
    return this->parameter;
}

/* *
 * 检测已经正确的参数的值格式
 */
bool CString::checkParameter(const std::string &parameter, std::string &value){
    if (!parameter.compare(NUMLINE) || !parameter.compare(NUMCOLUMN)){
        if (std::regex_match(value, std::regex("\\d+|l\\d+c\\d+"))){
            return true;
        }
        return false;
    }

    if (!parameter.compare(RECTANGLE)){
        if (std::regex_match(value, std::regex("\\{(\\d+ *, *){2}(\\d+|l\\d+c\\d+)\\}"))){
            //修正value值
            value = value.substr(value.find_first_not_of("{"), (value.at(0) == '{' ? value.size() - 2 : std::string::npos));
            return true;
        }
        return false;
    }

    //理论上使用不到
    return false;
}

/* *
 * 以不同常规颜色打印信息提示(这里为红色和蓝色)
 * pos == 0时，不输出表达式
 */
void CString::prompt(int pos, const std::string &errInfo, const std::string &which){
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);  
    WORD wOldColorAttrs;
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;  
      
    // Save the current color  
    GetConsoleScreenBufferInfo(h, &csbiInfo);  
    wOldColorAttrs = csbiInfo.wAttributes;  
    
    // Set the new color  
    SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_INTENSITY);  

    if (pos){
    std::cerr << this->getExpression() << std::endl;
        for (int i = 0; i < pos; i++)
            std::cerr << " ";
        std::cerr << "^" << std::endl;
    }
    std::cerr << which <<  errInfo << std::endl;
    
    // Restore the original color  
    SetConsoleTextAttribute(h, wOldColorAttrs);
#else
    if (pos){
        std::cerr << "\033[01;34m" << this->getExpression() << "\033[0m" << std::endl;
        for (int i = 0; i < pos; i++)
            std::cerr << " ";
        std::cerr << "\033[01;31m" << "^\033[0m" << std::endl;
    }
    std::cerr << "\033[01;31m" << which << errInfo << "\033[0m" << std::endl;
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
 * 处理'{'
 * info.step返回距离对应'}'的长度
 * info.description 描述次数信息
 *
 * 可能表达式中可能出现的一些错误：
 *  without }
 *  {d,dd,...}
 *  {not \d}
 */
CString::Info CString::braces(const std::string &str, int start){
    Info info;
    int pos = 0;
    std::vector<std::string> range;

    start++;

    if ((pos = str.find("}", start)) == std::string::npos){
        this->prompt(start - 1, "未发现'}'");
        return Info(true);
    }else {
        range = split(str.substr(start, pos - start), ",");

        //不是{d,dd}或{d}的情况
        //条件3 -- 取出括号内部字符串，检查第一个','后有没有','，防止{d,dd,}的情况
        if (range.size() == 0 || range.size() > 2 ||
            str.substr(start, pos - start).find(",", range[0].size() + 1) != std::string::npos){
            this->prompt(start + range[0].size() + range[1].size() + 1, "请使用一个或两个数字作为范围界定");
            return Info(true);
        }
        
        for (auto item : range){
            if (!std::regex_match(item, std::regex("\\d+"))){
                //未匹配数字
                this->prompt(str.find(item, start), "需要数字");
                return Info(true);
            }
        }
    }

    if (range.size() == 1){
        info.description.min_times = info.description.max_times = stoi(range[0]);
    }else {
        info.description.min_times = stoi(range[0]);
        info.description.max_times = stoi(range[1]);
    }
    info.step = pos - start + 1;

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

#if __TEST__ != 0
void output(std::queue<std::vector<Columns> > queue){
    while(!queue.empty()){
        auto line = queue.front();
        for (auto item : line){
            for (auto node : item.vNode){
                std::cout << "node: " << std::endl
                        << "str: " << node.str << std::endl;
                while (!node.description.empty()){
                    auto description = node.description.front();
                    std::cout << description.start << " " << description.end << " " << description.min_times << " " << description.max_times << std::endl;
                    node.description.pop();
                }
            }
            std::cout << "line   min: " << item.description.min_times << " max: " << item.description.max_times << std::endl;
        }
        std::cout << std::endl;
        queue.pop();
    }
}
#endif

/* *
 * 外用接口，用于解析字符串
 * 有种想用指针，舍弃多余部分的想法（说的就是你info = ?(exp, i), if...）
 */
std::queue<std::vector<Columns> > CString::parse(){
    if (this->expression.size() == 0){
        this->prompt(0, "表达式为空");
        return std::queue<std::vector<Columns> >();
    }

    std::queue<std::vector<Columns> > queue;
    std::vector<Columns> line(1);
    Columns::Node node = Columns::Node();
    //多用于记录字符宽度，下指针不用
    //之后，可以使用info.description代替 by 2016.08.01 02:15:21
    Columns::Node::Description description = Columns::Node::Description();
    Info info;
    auto iter = line.begin(), origin = iter;
    int roundBracketsCount = 0, step;                 //roundBracketsCount两层括号使用, step重新计算iter使用

    for (int i = 0; i < this->expression.size(); i++){
        switch(this->expression.at(i)){
        case '[':
            //std::cout << "[" << " " << i << std::endl;
            info = this->squareBrackets(this->expression, i);
            if (info.isError){
                return std::queue<std::vector<Columns> >();
            }
            
            //增加描述信息
            description.start = node.str.size();
            node.str += info.str;
            description.end = node.str.size() - 1;
            node.description.push(description);
            
            //到达对应的']'字符位置
            i += info.step;
            break;
        case '|':
            //std::cout << "|" << " " << i << std::endl;
            if (node.str.empty() || 
                (this->expression.at(i - 1) == '|' && node.str.back() != '|')){
                this->prompt(i, "多余的‘|’，忽略", "警告：");
            }else {
                iter->vNode.push_back(node);
                node = Columns::Node();
            }
            break;
        case '{':
            //std::cout << "{" << " " << i << std::endl;
            info = this->braces(this->expression, i);
            if (info.isError){
                return std::queue<std::vector<Columns> >();
            }
            //std::cout << "count: " << roundBracketsCount << std::endl;
            
            int npos;
            //修改描述信息
            if (this->expression.at(i - 1) != ')' 
               || this->expression.at(i - 2) == '\\' 
               || (npos = this->expression.find_last_not_of("\\", i - 2)) == std::string::npos 
               || (i - 2 - npos) % 2){
                //后者去除\)的情况(\\\))
                node.description.back().min_times = info.description.min_times;
                node.description.back().max_times = info.description.max_times;
                //std::cout << "pos: " << i << std::endl;
            }else {
                //')'
                if (roundBracketsCount == 0){
                    //第一层括号后面，与0比较，是因为已经减1，下同
                    queue.back().begin()->description.min_times = info.description.min_times;
                    queue.back().begin()->description.max_times = info.description.max_times;
                }else {
                    //第二层括号，可写交换后的origin，但是')'实现为origin = iter
                    line.back().description.min_times = info.description.min_times;
                    line.back().description.max_times = info.description.max_times;
                }
            }
            i += info.step;
            break;
        case '(':
            //std::cout << "(" << " " << i << std::endl;
            roundBracketsCount++;
            if (roundBracketsCount == 1){
                //第一层括号
                //此处可重载node operator== 与description operator==
                if (!node.str.empty()){
                    //不是新行
                    //防止"(xxxx)"
                    iter->vNode.push_back(node);
                    queue.push(line);

                    node = Columns::Node();
                    line = std::vector<Columns>(1);
                    origin = iter = line.begin();
                    //std::cout << "in" << std::endl;
                }
            }else if (roundBracketsCount == 2){
                //第二层括号
                /* *
                 * 增加替换符，将node入数组尾部(队尾?)，并在node中加入描述信息
                 * 在case ')'那里，使用iter = origin, node = iter->vNode.back()取回原记录值
                 */
                std::string symbol= std::string(secondRoundBrackets);
                description.start = node.str.size();
                node.str += symbol.replace(secondRoundBrackets.find(REPLACE), REPLACE.size(), std::to_string(line.size()));
                description.end = node.str.size() - 1;
                node.description.push(description);
                iter->vNode.push_back(node);
                node = Columns::Node();

                /* *
                 * 获取二维组行尾
                 * 例：当前有一行，则iter指向第二行
                 * 为什么使用行数替换REPLACE所指代的值，而不直接####之类？
                 *  1) 最初设计
                 *  2) 写到这里想到，可以通过本设计，增加括号嵌套
                 *  3) 2适用于想法是第几个"####"可以找寻第几行值
                 *  4) 注意一点：对迭代器resize(push resize pop之类)之后，iterator必会改变
                 */
                step = line.size() - 1;
                line.push_back(Columns());
                origin = std::next(line.begin(), step);
                iter = std::prev(line.end(), 1);
            }else {
                //一般在这里不会被使用
                this->prompt(i, "多余的'('");
                return std::queue<std::vector<Columns> >();
            }
            break;
        case ')':
            //std::cout << ")" << " " << i << std::endl;
            roundBracketsCount--;
            if (roundBracketsCount == 1){
                //第二层括号
                iter->vNode.push_back(node);
                iter = origin;
                node = iter->vNode.back();
            }else if (roundBracketsCount == 0){
                //第一层括号
                //需要更改队尾的node节点，而非iter->vNode.push_back(node);
                iter->vNode.back() = node;
                queue.push(line);

                node = Columns::Node();
                line = std::vector<Columns>(1);
                origin = iter = line.begin();
            }else {
                this->prompt(i, "括号数不匹配");
                return std::queue<std::vector<Columns> >();
            }
            break;
        case '\\':
            //std::cout << "\\" << " " << i << std::endl;
            if (binder.find(this->expression.at(i + 1)) != binder.end()){
                description.start = node.str.size();
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
                return std::queue<std::vector<Columns> >();
            }
            break;
        case '-':   //想使用，请使用转义字符'\'
        case '}':
        case ']':
            this->prompt(i, "多余字符");
            return std::queue<std::vector<Columns> >();
        default:
            //std::cout << "default " << i << " " << this->expression.at(i) << std::endl;
            description.start = node.str.size();
            node.str += this->expression.at(i);
            description.end = node.str.size() - 1;
            node.description.push(description);
            break;
        }
    }
    //()匹配问题
    if (roundBracketsCount != 0){
        this->prompt(this->expression.find_last_of("("), "没有找到对应的')'");
        return std::queue<std::vector<Columns> >();
    }

    //可能含有node未入数组的情况
    //出现在default之类结束的情况
    if (node.str.size()){
        iter->vNode.push_back(node);
        queue.push(line);
    }

#if __TEST__ != 0
    output(queue);
#endif

    return queue;
}
