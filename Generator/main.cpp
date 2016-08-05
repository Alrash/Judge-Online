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
#include <queue>
#include <ctime>
#include <map>
#include <algorithm>
#include <regex>
#include "cstring.h"
#include "config.h"
#include "generator.h"
#include "functions.h"

using namespace std;

string str;

struct ddd{
	string str;
};

int main(int argc, char *argv[]){
    /*if (argc == 1){
        cout << "输入表达式：";
        getline(cin, str);
    }else {
        for (int i = 1; i < argc; i++){
            str += argv[i];
            str += " ";
        }
    }*/

	/*
	for (auto it : split(argv[1] + 1, "=")){
		cout << it << "@" << endl;
	}*/
	
	//CString("daksdaskd[ ]asdasnd -asda=sd --asdnjd=asdnj -numline=4 -numline");
	cout << "输入: " << endl;
	//cin >> str;
    vector<vector<string>> test;
    test = _regex_search("1,3,15", "(\\d+,){2}(\\d+)");
    for (auto line : test){
        for (auto item : line){
            cout << item << " ";
        }
        cout << endl;
    }
    cout << test.empty() << stoi(*prev(test[0].end(), 1)) << endl;
	CString cstring =  CString();
    Generator generator = Generator();
    generator.setUnit(cstring.parse(), map<string, string>());

    return 0;
}
