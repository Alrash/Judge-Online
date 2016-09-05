/*************************************************************************
	> File Name: compiler.cpp
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Sat 03 Sep 2016 07:50:15 PM CST
 ************************************************************************/

#include <vector>
#include <string>
#include <map>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include "../public/functions.h"
#include "../public/log.h"
#include "../public/config.h"

#include <iostream>
using namespace std;

/* *
 * 这里参数没有检测参数值
 */
std::vector<std::string> compiler_parameter = {
    "-access_path", "-compiler", "-error_path", "-extension", 
    "-script", "-sid"
};

std::map<std::string, std::string> parameter_map = {};

int init(int argc,char* argv[]){
    int i = 1;

    for (; i < argc; i++){
        if (!is_find_in_vector(compiler_parameter, argv[i])){
            break;
        }
        if (argv[i + 1][0] == '-'){
            parameter_map[argv[i] + 1] = "";
        }else {
            parameter_map[argv[i] + 1] = argv[i + 1];
            ++i;
        }
    }

    if (i < argc){
        if (!parameter_map["error_path"].empty()){
            Log log = Log(parameter_map["error_path"].c_str());
            char error_message[1024];
            sprintf(error_message, "paramter %s is not existed", argv[i]);
            log.append(error_message, std::string(parameter_map["sid"] + " call compiler function").c_str());
            return -1;
        }
    }

    if (parameter_map.size() != compiler_parameter.size()){
        if (!parameter_map["error_path"].empty()){
            Log log = Log(parameter_map["error_path"].c_str());
            std::string error_message = std::string("less parameter, and now parameter maps");
            for (auto item : parameter_map){
                error_message += std::string(" " + item.first + " " + item.second);
            }
            log.append(error_message.c_str(), std::string(parameter_map["sid"] + " call compiler function").c_str());
            return -1;
        }
    }

    parameter_map["filname"] = "Main." + parameter_map["extension"];
    if (access(std::string("temp." + parameter_map["extension"]).c_str(), F_OK) == -1){
        Log log = Log(parameter_map["error_path"].c_str());
        char error_message[1024];
        sprintf(error_message, "file temp.%s is not existed", parameter_map["extension"].c_str());
        log.append(error_message, std::string(parameter_map["sid"] + " call compiler function").c_str());
        return -1;
    }

    return 0;
}

/* *
 * 进行脚本处理，之后进行编译
 * 若编译错误，返回-1
 */
int compiler(){
    system(parameter_map["script"].c_str());
    Log log = Log(parameter_map["access_path"].c_str());

    if (!parameter_map["compiler"].empty()){
	int status = system(parameter_map["compiler"].c_str());
        if (!WIFEXITED(status) || WEXITSTATUS(status)){
            log.append("compiler error", std::string(parameter_map["sid"] + " call compiler function").c_str());
            return COMPILER_ERROR;
        }
    }

    log.append("compiler success", std::string(parameter_map["sid"] + " call compiler function").c_str());
    return 0;
}

/* *
 * 执行编译，并判断是否编译错误
 * 编译错误， 返回COMPILER_ERROR(定义在config.h中)
 * 初始化错误，返回INIT_ERROR(定义在config.h中)
 */
int main(int argc, char* argv[]){
    if (init(argc, argv) == -1){
        return INIT_ERROR;
    }
    return compiler();
}
