/*************************************************************************
	> File Name: readConfig.cpp
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Wed 31 Aug 2016 11:55:46 AM CST
    > Function: 
    > Notice: link add `pkg-config glib-2.0 --cflags --libs` parameter
              without using has_*** to check, exit if having error
 ************************************************************************/

#include <cstdio>
#include <cstring>
#include "readConfig.h"
#include "log.h"

ReadConfig::ReadConfig(){
    strcpy(file, "/etc/judge/conf.d/judge.conf");
    loadFile();
}

ReadConfig::ReadConfig(const char* filename){
    strcpy(file, filename);
    loadFile();
}

void ReadConfig::loadFile(){
    config = g_key_file_new();
    error = nullptr;
    if (!g_key_file_load_from_file(config, file, flags, &error)){
        fprintf(stderr, "%s\n", error->message);
        exit(EXIT_FAILURE);
    }
}

ReadConfig::~ReadConfig(){
    g_key_file_free(config);
}

std::vector<std::string> ReadConfig::getGroupKeys(const char* group_name){
    std::vector<std::string> keys = std::vector<std::string>();
    gsize length;
    error = nullptr;
    gchar** key = g_key_file_get_keys(config, group_name, &length, &error);
    if (isError()){
        this->exit_failure(error->message);
        return std::vector<std::string>();
    }

    for (int i = 0; i < length; i++){
        keys.push_back(key[i]);
    }

    return keys;
}

std::map<std::string, std::string> ReadConfig::getGroupMaps(const char* group_name){
    auto maps = std::map<std::string, std::string>();
    auto keys = this->getGroupKeys(group_name);
    for(auto it : keys){
        maps[it] = this->getKeyValue(group_name, it.c_str());
    }

    return maps;
}

std::string ReadConfig::getKeyValue(const char* group_name, const char* key){
    error = nullptr;
    gchar* value = g_key_file_get_string(config, group_name, key, &error);
    if (isError()){
        this->exit_failure(error->message);
        return std::string();
    }

    return value;
}

std::vector<std::string> ReadConfig::getGroups(){
    std::vector<std::string> group_name = std::vector<std::string>();
    gsize length;
    gchar** groups = g_key_file_get_groups(config, &length);

    for (int i = 0; i < length; i++){
        group_name.push_back(groups[i]);
    }

    return group_name;
}

/*
 * 考虑到某些问题，这里的提示，没有写入文件
 * **实际上是不知道使用ReadConfig在此声明调用的后果**
 * */
void ReadConfig::exit_failure(const char* error_message){
    //char s[1024];
    fprintf(stderr , "from ReadConfig -- %s", error_message);
    
    /*
    Log log("");
    log.append(s, "error");
    */
}
