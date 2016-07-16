/*************************************************************************
	> File Name: compiler.cpp
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Sun 10 Jul 2016 10:47:34 PM CST
    > Function: 用于修正和编译源文件，目前编译选项与脚本静态编译至本文件内
                2016/07/11 22:11:44
    > Defect: 没有日志系统，较弱的出错处理，扩展性差、需重复编译
 ************************************************************************/

#include <iostream>
#include <string>
#include <stdio.h>         //sprintf函数使用
#include <unistd.h>        //execl函数使用
#include <vector>
#include "../public/functions.h"
#include "../public/config.h"

using namespace std;

//编译选项字符串
//编译文件文件名统一改为Main.xxx，编译后可执行文件名为Main
//python不执行编译,编译选项置为空字符串(直接置为NULL，会抛出str::logic_error)
const vector<string> compiler = {
    "gcc -Wall -lm -O2 -o i ${root}/check/Main ${root}/check/Main.c 2> ${root}/check/err.out",
    "g++ -Wall -lm -O2 -o ${root}/check/Main ${root}/check/Main.cpp 2> ${root}/check/err.out",
    "g++ -std=c++11 -Wall -lm -O2 -o ${root}/check/Main ${root}/check/Main.cpp 2> ${root}/check/err.out",
    "javac ${root}/check/Main.java 2> ${root}/check/err.out",
    ""    
};

//编译选项的长度
//暂时没有用处 by Alrash 2016/07/10 22:56:00
//现将compiler变量类型设置为向量，可自动获取长度(compiler.size())
//    本项可以选择使用
const int choise_len = compiler.size();

//删除java源文件中的package指令和替换public class 名
//使用范例：sprintf(buff, java_sed_scrpit.c_str(), "/www", 25252,"java.java", "www", "Main.java");
const string java_sed_script  = "sed -re '/package.*/d;s/[ \t]*public[ \t]+class[ \t]+[^{]*/public class Main/g' %s/%d/%s > %s/check/Main.java";

//删除c或cpp源文件中的包含sys/*或unistd.h的头文件
//五个待填充的位置，前三个为待检查文件的绝对路径，后两个分别为root/check/Main.extension
const string cpp_sed_script = "sed -re '/sys\\/.*/d;/\/dev\/.*/d;/unistd\\.h/d' %s/%d/%s > %s/check/Main.%s";

//python替换脚本
const string python_sed_script = "sed -re '/import[ \t]*os/g' %s/%d/%s > %s/check/Main.py";

//获取argv的几个变量
int style, sid;
string file;

//其余全局变量，初始化在init函数内
string filename, extension;
char script[1025];
char source_path[256], destination_path[256];

/* *
 * 初始化变量，并检测文件是否存在
 * 暂时未使用映射做初始化工作
 */
int init(){
    //切割字符串，获得文件名与扩展文件名
    //不仅格式化脚本会使用，检测文件存在性时，也需要
    vector<string> vt = split(file, ".");
    filename = vt[0];
    extension = vt[1];

    //格式化脚本字符串
    switch(style){
    //0-2代表c、c++、c++11
    case 0:
    case 1:
    case 2:
        sprintf(script, cpp_sed_script.c_str(), root.c_str(), sid, file.c_str(), root.c_str(), extension.c_str());
        break;
    //java
    case 3:
        sprintf(script, cpp_sed_script.c_str(), root.c_str(), sid, file.c_str(), root.c_str());
        break;
    //python3.5
    case 4:
        sprintf(script, cpp_sed_script.c_str(), root.c_str(), sid, file.c_str(), root.c_str());
        break;
    default:
        script[0] = '\0';
        break;
    }

    //获得源文件与目标文件绝对路径
    sprintf(source_path, "%s/%d/%s", root.c_str(), sid, file.c_str());
    sprintf(destination_path, "%s/check/Main.%s", root.c_str(), extension.c_str());

    //检查文件是否存在
    if (access(source_path, F_OK) == -1){
        perror("access");
        return -1;
    }

    return 0;
}

/* *
 * 执行脚本，并进行编译
 */
int compiler_function(){
    //使用子进程进行脚本替换处理
    //使用子进程的原因：使用execl后，执行的脚本会替换本进程的所有资源，之后的代码均不执行
    //可以使用system(char *command)代替
    int pid = fork();
    if (pid == 0){
        //执行脚本
        execl("/bin/sh", "sh", "-c", script, NULL);
        exit(0);
    }

    //等待子进程结束
    wait();

    if (access(destination_path, F_OK) == -1){
        perror("access");
        return -1;
    }

    /*
    //python文件没有(未使用chmod)，暂时注释
    //并且，具有可执行权限的是编译后的文件，而不是destination_path所标识的文件
    //查看文件是否具有可执行权限
    if (access(destination_path, X_OK) == -1){
        perror("access");
        return -1;
    }
    */

    if (extension != "py"){
        //因为使用的静态标识符const，所以compiler不能改变
        string choise = compiler[style];
        int pos = -1;

        while ((pos = choise.find("${root}")) != string::npos){
            choise.replace(pos, 7, root);
        }

        //开子进程进行编译
        int _pid = fork();
        if (_pid == 0){
            execl("/bin/sh", "sh", "-c", choise.c_str(), NULL);
            exit(0);
        }

        wait();
    }

    return 0;
}

/* *
 * 这样的写法，main函数的参数至少有一个，及./main之类(0)
 * 其余参数设置及解释：
 * 0-n:表示是哪一种源文件需要进行编译，具体值为编译选项的偏移量(1)
 * d*:提交号(2)
 * filename.extension:完整文件名及后缀(3)
 */
int main(int argc, char *argv[]){

    if (argc != 4){
        cout << "param errer" << endl;
        exit(-1);
    }

    //下次更新，增加检测函数
    //instanceof之类
    style = atoi(argv[1]);
    sid = atoi(argv[2]);
    file = argv[3];

    if (init() == -1){
        cout << "init error" << endl;
        exit(-1);
    }

    if (compiler_function() == -1){
        cout << "compiler error" << endl;
        exit(-1);
    }

    return 0;
}
