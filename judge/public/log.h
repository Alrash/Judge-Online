/*************************************************************************
	> File Name: log.h
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Wed 31 Aug 2016 10:43:55 PM CST
 ************************************************************************/

#ifndef _LOG_H
#define _LOG_H

class Log{
public:
    Log();
    explicit Log(const char* file);

    void append(const char* appendContent, const char* prefix);
private:
    char filename[256];
    bool exist;

    void init(const char* filename, bool full = false);
    //传入s串长度，需要超过21
    char* getNowLocaltime(char* s);
};

#endif
