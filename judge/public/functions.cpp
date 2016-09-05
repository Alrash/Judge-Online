/*************************************************************************
	> File Name: functions.cpp
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Mon 11 Jul 2016 10:20:45 PM CST
 ************************************************************************/

#include <vector>
#include <string>
#include <regex>

std::vector<std::string> split(const std::string &str, const std::string &pattern){
    //存放结果
    std::vector<std::string> result;
    int pos = 0, size = str.size();

    //提取并存放结果
    for (int i = 0; i < size; i++){
        pos = str.find(pattern, i);

        if (pos < size && pos != std::string::npos){
            result.push_back(str.substr(i, pos - i));
            i = pos + pattern.size() - 1;
        }else{
            if (i < size)
                result.push_back(str.substr(i));
            break;
        }
    }

    return result;
}

//替换所有substr所指字符串
std::string replaceAll(const std::string &str, const std::string &substr, const std::string &repstr){
    std::string result = str;
    int len = substr.size(), pos;

    while ((pos = result.find(substr)) != std::string::npos){
        result.replace(pos, len, repstr);
    }
    
    return result;
}

/* *
 * 检测字符串是否为非0开头的纯数字
 */
bool is_numeric(std::string value){
    return std::regex_match(value, std::regex("^[1-9]\\d*$"));
}

/* *
 * 检查vector中是否含有字符串str
 */
bool is_find_in_vector(std::vector<std::string> vt, std::string str){
    for (auto item : vt){
        if (item.compare(str) == 0){
            return true;
        }
    }
    return false;
}
