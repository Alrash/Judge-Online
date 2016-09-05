/*************************************************************************
	> File Name: readConfig.h
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Wed 31 Aug 2016 02:21:03 PM CST
 ************************************************************************/

#ifndef _READCONFING_H
#define _READCONFING_H

#include <glib.h>
#include <string>
#include <map>
#include <vector>

const GKeyFileFlags flags = (GKeyFileFlags)(G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS);

class ReadConfig{
public:
    ReadConfig();
    explicit ReadConfig(const char* filename);
    ~ReadConfig();

    std::vector<std::string> getGroupKeys(const char* group_name);
    std::string getKeyValue(const char* group_name, const char* key);
    std::map<std::string, std::string> getGroupMaps(const char* group_name);
    std::vector<std::string> getGroups();
private:
    char file[256];
    GKeyFile* config;
    GError* error;

    void loadFile();
    void exit_failure(const char* error_message);

    bool isError(){
        if (error != nullptr)
            return true;
        return false;
    }
};

#endif
