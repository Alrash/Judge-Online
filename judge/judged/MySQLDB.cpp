/*************************************************************************
	> File Name: MySQLDB.cpp
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Wed 20 Jul 2016 10:46:03 PM CST
 ************************************************************************/

#include <string>
#include <cstdio>
#include "MySQLDB.h"

MySQLDB::MySQLDB(){
    mysqli = mysql_init(NULL);
}

MySQLDB::~MySQLDB(){
}

int MySQLDB::init(){
    if (mysql_real_connect(mysqli, HOST, USERNAME, PASSWORD, DATABASE, PORT, NULL, 0) == NULL){
        //写日志，转存或者不变测试结果
        fprintf(stderr, "mysql connect error: %s\n", mysql_error(mysqli));
        return -1;
    }

    std::string encoding = "set names \'UTF8\'";
    if (mysql_real_query(mysqli, encoding.c_str(), encoding.size())){
        fprintf(stderr, "set coding error: %s\n", mysql_error(mysqli));
        return -1;
    }
    
    return 0;
}

int MySQLDB::close(){
    mysql_close(mysqli);
    return 0;
}

bool MySQLDB::update(std::string sql){
    if (mysql_real_query(mysqli, sql.c_str(), sql.size())){
        fprintf(stderr, "%s error: \n", sql.c_str(), mysql_error(mysqli));
        return false;
    }
    return true;
}
