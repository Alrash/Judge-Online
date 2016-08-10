/*************************************************************************
    > File Name: main.cpp
    > Author: Alrash
    > Mail: kasukuikawai@gmail.com
    > Created Time: Sat 23 Jul 2016 02:01:19 PM CST
    > Function: 生成测试输入用例
                    利用部分基础正则表达式产生
                    仅支持圆括号两层嵌套，比如ip地址正则匹配
 ************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <cstdio>
#include "cstring.h"
#include "config.h"
#include "generator.h"
#include "functions.h"

using namespace std;

map<string, string> main_parameter = {
    {"times", "1"},
    {"exp", ""},
    {"h", ""},
    {"-help", ""}
};

int readfile(){
    FILE *fp;
    char str[1025];

    if ((fp = fopen("help", "r")) == NULL){
        cerr << "文件不存在" << endl;
        return -1;
    }

    while(!feof(fp)){
        fgets(str, 1025, fp);
        cout << str;
    }

    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]){
    int num = 0;
    string expression = "\"", str;
    CString cstring("t");           //用于下面自动获取数据类型，而不报错使用
    Generator generator;
    auto parse = cstring.parse();   //主要返回队列的类型

    if (argc != 1){
        vector<string> vts;
        for (int i = 1; i < argc; i++){
            vts = split(argv[i] + 1, "=");
            if (main_parameter.find(vts[0]) != main_parameter.end()){
                if (!vts[0].compare("h") || !vts[0].compare("-help")){
                    readfile();
                    return 0;
                }

                if (!vts[0].compare("exp")){
                    for (int i = 1; i < vts.size() - 1; i++){
                        main_parameter["exp"] += vts[i] + "=";
                    }
                    main_parameter["exp"] += vts.at(vts.size() - 1);
                    continue;
                }

                if (vts.size() > 2){
                    cerr << "参数错误：" << argv[i] << endl;
                    continue;
                }

                if (!vts[0].compare("times")){
                    if (regex_match(vts[1], regex("\\d+"))){
                        main_parameter[vts[0]] = vts[1];
                    }else {
                        cerr << "参数值形式错误：" << argv[i] << endl;
                    }
                }
            }else {
                cerr << "忽略参数：" << argv[i] << endl;
            }
        }
    }else {
        int times;
        cout << "输出文件数量：";
        cin >> times;
        main_parameter["times"] = to_string(times);
        getchar();
    }

    generator.setCount(stoi(main_parameter["times"]));
    
    if (!main_parameter["exp"].compare("")){
        while (true){
            num++;
            cout << "请输入第" << num << "行表达式及参数: " << endl;
            getline(cin, str);
            if (!str.compare("end")){
                break;
            }
            cstring.setExpression(str);
            parse = cstring.parse();
            if (parse.empty()){
                exit(EXIT_FAILURE);
            }
        
            generator.setUnit(parse, cstring.getParameter());
            expression += cstring.getExpression() + " --link ";
        }
    }else {
        expression += main_parameter["exp"];
        for (auto item : split(main_parameter["exp"], "--link")){
            cstring.setExpression(item);
            parse = cstring.parse();
            if (parse.empty()){
                exit(EXIT_FAILURE);
            }
        
            generator.setUnit(parse, cstring.getParameter());
        }
    }
    
    generator.generator();
    expression += "\"";
    generator.fileAppend(string(generator.getDir() + "expression.txt").c_str(), expression, "w");

    return 0;
}
