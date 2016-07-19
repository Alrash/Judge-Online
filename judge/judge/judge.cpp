/*************************************************************************
	> File Name: judge.cpp
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Tue 19 Jul 2016 12:01:29 AM CST
 ************************************************************************/

#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include "../public/config.h"

using namespace std;

const string pipe_path = root + "/judge.pipe";
const int open_mode = O_WRONLY | O_NONBLOCK;

/* *
 * 需要参数：
 * 本身提交sid (1)
 * 提交语言style (2)
 * 问题号pid (3)
 * 问题测试文件数count (4)
 * 问题限制时间sec (float/double, s, 5)
 * 问题限制内存大小memory (M, 6)
 */
int main(int argc, char *argv[]){
    int pipeid = -1;
    string buff = "";

    for (int i = 1; i < argc; i++){
        //不这样写，有神秘加成-_-|||
        buff += argv[i];
        buff += splitch; 
    }
    buff.resize(PIPE_LEN, fillch);

    //管道文件不存在，创建他
    if (access(pipe_path.c_str(), F_OK) == -1){
        //改变掩码值
        mode_t origin = umask(0666);
        if (mkfifo(pipe_path.c_str(), 0666) != 0){
            //不能创建文件，退出
            fprintf(stderr, "can not craete fifo %s\n", pipe_path.c_str());
            exit(EXIT_FAILURE);
        }
        //还原掩码值
        umask(origin);
    }

    //保证能够打开管道文件
    while(true){
        if ((pipeid = open(pipe_path.c_str(), open_mode)) != -1){
            //打开管道文件
            if (write(pipeid, buff.c_str(), PIPE_LEN) == 1){
                fprintf(stderr, "write error\n");
                exit(EXIT_FAILURE);
            }
            close(pipeid);
            break;
        }
    }

    return 0;
}
