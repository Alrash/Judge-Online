/*************************************************************************
	> File Name: log.cpp
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Wed 31 Aug 2016 10:52:45 PM CST
 ************************************************************************/

#include <cstring>
#include <cstdio>
#include <string>
#include <unistd.h>
#include <ctime>
#include "readConfig.h"
#include "log.h"

Log::Log(){
    ReadConfig config = ReadConfig();
    auto access_file = config.getKeyValue("log", "access");
    init(access_file.c_str());
}

Log::Log(const char* filename){
    init(filename, true);
}

void Log::init(const char* filename, bool full){
    this->exist = true;

    if (full == false){
        ReadConfig config = ReadConfig();
        auto path = config.getKeyValue("log", "path");
        sprintf(this->filename, "%s/%s", path.c_str(), filename);
    }else {
        sprintf(this->filename, "%s", filename);
    }

    if (access(this->filename, W_OK) == -1){
        this->exist = false;
    }
} 

void Log::append(const char* appendContent, const char* prefix){
    if (this->exist == false){
        fprintf(stderr, "not such a file\n");
        return;
    }
    
    char *now = new char[30];
    now = this->getNowLocaltime(now);
    
    FILE *fp;
    fp = fopen(this->filename, "a");
    fprintf(fp, "[%s] %s: %s\n", now, prefix, appendContent);
    fclose(fp);

    delete[] now;
}

char* Log::getNowLocaltime(char* s){
    time_t now;;
    struct tm *info;

    time(&now);
    info = localtime(&now);
    sprintf(s, "%4d-%02d-%02d %02d:%02d:%02d", info->tm_year + 1900, info->tm_mon + 1, info->tm_mday,
           info->tm_hour, info->tm_min, info->tm_sec);

    return s;
}
