/*************************************************************************
	> File Name: judged.cpp
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Mon 18 Jul 2016 08:29:41 PM CST
    > 还需要数据库写入功能
 ************************************************************************/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string>
#include <vector>
#include <stdio.h>
#include "../public/config.h"

using namespace std;

//下次拿...声明吧-_-|||
int splitString(const string &buff, int &sid, int &style, int &pid, int &times, int &isFormat, int &sec, int &memory);
int running(int sid, int style, int times, int isFormat, int sec, int memory, const vector<string> &command_check);

char path[512];
char compiler_path[550];
char executor_path[550];

int main(){
    daemon(1, 0);

    char buff[PIPE_LEN + 1];
    int open_mode = O_RDONLY;
    int pipe_id;
    string pipe_path = root + "/judge.pipe";
    string log_path = "log.txt";

    FILE *fp;

    if ((fp = fopen(log_path.c_str(), "a")) == NULL){
        exit(EXIT_FAILURE);
    }

    if (getcwd(path, 512) == NULL){
        fprintf(fp, "get pwd error\n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }
    sprintf(compiler_path, "%s/compiler", path);
    sprintf(executor_path, "%s/executor", path);

    fprintf(fp, "%s\n", path);

    if (access(pipe_path.c_str(), F_OK) == -1){
        mode_t origin = umask(0666);
        if (mkfifo(pipe_path.c_str(), 0666) != 0){
            fprintf(fp, "Could not create fifo %s\n", pipe_path.c_str());
            fclose(fp);
            exit(EXIT_FAILURE);
        }
        umask(origin);
    }

    if ((pipe_id = open(pipe_path.c_str(), open_mode)) == -1){
        fprintf(fp, "Could not open fifo %s\n", pipe_path.c_str());
        fclose(fp);
        exit(EXIT_FAILURE);
    }
    fclose(fp);

    int ret = 0;
    int pid, style, sid, times, isFormat, sec, memory;
    char command[512];
    string check_path = root + "/check";
    vector<vector<string> > command_check = {
        {"%d.c", "c", "Main"},
        {"%d.cpp", "c", "Main"},
        {"%d.cpp", "c", "Main"},
        {"%d.java", "java", "Main.class"},
        {"%d.py", "python", "Main.py"}
    };

    while(true){
        ret = read(pipe_id, buff, PIPE_LEN);
        
        if (ret == -1){
            fp = fopen(log_path.c_str(), "a");
            fprintf(fp, "read error\n");
            fclose(fp);
            continue;
        }

        if (ret == 0)
            continue;

        /* *
         * 以下主要任务：拆解buff，执行编译、运行程序
         */
        if (splitString(buff, sid, style, pid, times, isFormat, sec, memory)){
            fp = fopen(log_path.c_str(), "a");
            fprintf(fp, "split string error, %s\n", buff);
            fclose(fp);
            continue;
        }

        //复制测试文件内容
        sprintf(command, "cp %s/%d/in/* %s", root.c_str(), pid, check_path.c_str());
        system(command);
        sprintf(command, "cp %s/%d/out/* %s", root.c_str(), pid, check_path.c_str());
        system(command);
        sprintf(command, "cp %s/%d/* %s", root.c_str(), sid, check_path.c_str());
        system(command);

        running(sid, style, times, isFormat, sec, memory, command_check[style]);

        //清空测试信息
        sprintf(command, "rm -rf %s/*", check_path.c_str());
        system(command);

        /*
        //test
        if (ret){  
            fp = fopen(log_path.c_str(), "a");
            fprintf(fp, "%s\n", buff);
            fclose(fp);
        }*/
    }
}

int splitString(const string &buff, int &sid, int &style, int &pid, int &times, int &isFormat, int &sec, int &memory){
    int pos = 0, npos;
    
    npos = buff.find(splitch, pos);
    sid = stoi(buff.substr(pos, npos - pos));
    
    pos = npos + 1;
    npos = buff.find(splitch, pos);
    style = stoi(buff.substr(pos, npos - pos));
    
    pos = npos + 1;
    npos = buff.find(splitch, pos);
    sid = stoi(buff.substr(pos, npos - pos));
    
    pos = npos + 1;
    npos = buff.find(splitch, pos);
    times = stoi(buff.substr(pos, npos - pos));

    pos = npos + 1;
    npos = buff.find(splitch, pos);
    isFormat = stoi(buff.substr(pos, npos - pos));
    
    pos = npos + 1;
    npos = buff.find(splitch, pos);
    sec = stof(buff.substr(pos, npos - pos)) * 1000;
    
    pos = npos + 1;
    npos = buff.find(splitch, pos);
    memory = stoi(buff.substr(pos, npos - pos));

    return 0;
}

/* *
 * 下次更新使用指针重写
 */
int running(int sid, int style, int times, int isFormat, int sec, int memory, const vector<string> &command_check){
    //这里的pid为进程号，而非问题号
    int pid = 0;
    int status;

    if ((pid = fork()) == -1){
        //写日志
        return -1;
    }

    if (pid == 0){
        //子进程编译
        char file[256];
        sprintf(file, command_check[0].c_str(), sid);
        
        execl(compiler_path, "./compiler", to_string(style).c_str(), to_string(sid).c_str(), file, NULL);

        //执行错误写日志
        
        exit(0);
    }else {
        waitpid(pid, &status, 0);

        if (access(command_check[2].c_str(), F_OK) == -1){
            //没有编译后的文件，退出
            //写日志
            return -1;
        }

        if ((pid = fork()) == -1){
            //写日志
            return -1;
        }

        if (pid == 0){
            //子进程编译
            execl(executor_path, "./executor", command_check[1].c_str(), to_string(times).c_str(), to_string(isFormat).c_str(), to_string((float)sec / 1000).c_str(), to_string(memory).c_str(), NULL);

            //执行错误写日志

            exit(0);
        }else {
            waitpid(pid, &status, 0);

            //读文件，数据库操作
            //暂时未写
            /**for test*/
            char resultpath[256], str[512];
            sprintf(resultpath, "%s/check/result", root.c_str());
            FILE *fp, *fa;            
            fp = fopen("temp.txt", "a");
            fa = fopen(resultpath, "r");
            while(!feof(fa)){
            	fgets(str, 512, fa);
            	fprintf(fp, "%s", str);
            }
            fclose(fa);
            fclose(fp);
        }
    }

    return 0;
}
