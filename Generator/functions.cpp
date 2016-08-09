/*************************************************************************
	> File Name: functions.cpp
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Mon 11 Jul 2016 10:20:45 PM CST
 ************************************************************************/

#include <vector>
#include <string>
#include <iostream>

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
