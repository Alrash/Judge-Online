/*************************************************************************
	> File Name: MySQLDB.h
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Wed 20 Jul 2016 10:33:36 PM CST
 ************************************************************************/

#ifndef _MYSQLDB_H
#define _MYSQLDB_H

#include <mysql/mysql.h>
#include <string>
#include "../public/config.h"

class MySQLDB{
public:
    MySQLDB();
    ~MySQLDB();

    int init();
    bool update(std::string sql);
    int close();
private:
    MYSQL *mysqli;
};

#endif
