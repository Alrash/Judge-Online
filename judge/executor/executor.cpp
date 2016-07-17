/*************************************************************************
	> File Name: executor.cpp
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Thu 14 Jul 2016 11:00:25 PM CST
    > 三流检测，仅提供思路 orz
    > Functions: 执行已经编译好的代码，使用重定向保存文件，等待检测
                 检测结果保存至result中，文件格式：？
 ************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/ptrace.h>
#include <string.h>
#include "check.h"
#include "../public/config.h"
#include "../public/functions.h"

using namespace std;

//kill子进程使用
//因为void name(int) 固定函数使用
int pid;
//几乎同上，用于设置限制时间
int limit_time, limit_time_origin;
int style, count, limit_memory;
char resultfile[126] = "result";
string extension = "", executor_file_path = "";
bool isFormat = false;

//缩放比例
const int psize = getpagesize() / 1024;

/* *
 * 执行选项的一部分，重定向之后拼接
 */
const vector<vector<string> > executor_choise = {
    {"${root}/check/Main", "./Main"},
    {"/usr/bin/java", "java Main"},
    {"/usr/bin/python3.5", "python3.5 Main.py"}
};

/* *
 * 定时器
 * 用于限制用户时间，触发后kill掉子进程，之后取消定时器
 */
void timer(int signo){
    kill(pid, SIGKILL);
    //alarm(limit_time);
    alarm(0);
}

/* *
 * 用于设置各种限制，超出限制时，使用WIFSIGNALED检测，WTERMSIG获取实际信号
 * @param:
 *       resource见下, limit限制资源大小的具体值
 * 
 * 本程序设置CPU时间RLIMIT_CPU，实际错误信号SIGXCPU (s)
 *           输出文件大小RLIMIT_FSIZE，实际错误信号SIGXFSZ (B)
 *           堆栈内存限制RLIMIT_AS，实际错误信号SIGSEGV (B)
 *           打开文件限制RLIMIT_NOFILE，不知道 (k + 1)
 * 既使用RLIMIT_CPU，又使用alarm(): 防止sleep之类的函数
 */
int setlimit(int resource, int limit){
    struct rlimit lim;
    lim.rlim_cur = lim.rlim_max = limit;
    if (setrlimit(resource, &lim) != 0){
        perror("setrlimit");
        cout << limit << endl;
        exit(-1);
    }
    return 0;
}

/* *
 * 保存结果
 * 运行时间单位ms，使用内存单位B
 * 由于int型的范围，memory一般只能记录2G一下的内存
 */
int saveResult(bool isRight, int answer, int current_time, int memory){
    FILE *fp;
    if ((fp = fopen(resultfile, "a")) == NULL){
        cout << "can't open file" << endl;
        return -1;
    }

    fprintf(fp, "%c %d %d %d\n", isRight ? 'T' : 'F', answer, current_time, memory);

    fclose(fp);
    return 0;
}

/* *
 * init函数，初始化配置字符串
 * 需要切换目录，和改变用户
 * 暂时没有更改用户 2016/07/15 22:31:41
 */
int init(){
    //获取限制时间
    if (limit_time_origin % 1000 == 0){
        //整数时间
        limit_time = limit_time_origin / 1000;
    }else{
        //正秒不到的时间，去大于本时间的最小整数
        limit_time = limit_time_origin / 1000 + 1;
    }

    if (extension == "c"){
        style = 0;
        executor_file_path = executor_choise[style][0];
        executor_file_path = replaceAll(executor_file_path, "${root}", root);
    }else if (extension == "java"){
        style = 1;
        executor_file_path = executor_choise[style][0];
    }else if (extension == "python"){
        style = 2;
        executor_file_path = executor_choise[style][0];
    }else {
        return -1;
    }

    string workdir = root + "/check";

    //切换工作目录至path所指之处
    if (chdir(workdir.c_str()) == -1){
        perror("executor chdir");
        return -1;
    }

    return 0;
}

/* *
 * 设置子进程的各种限制
 */
int setlimits(){
    setlimit(RLIMIT_CPU, limit_time);                        //限制CPU时间
    setlimit(RLIMIT_AS, limit_memory * MB);                  //内存限制limit_memory M
    setlimit(RLIMIT_FSIZE,  max_outfile_size);               //输出文件最大1M
    setlimit(RLIMIT_NOFILE, 10);                             //打开文件数11
    return 0;
}

/* *
 * 重定向标准输入、输出、错误流
 * 备注：
 *       无论是否含有输入项，必须要有输入文件
 *       其余两个文件 -- 没有必要
 */
int reopen(int num){
    char infile[55], outfile[55], errfile[55];

    sprintf(infile, "input%.2d.txt", num);
    sprintf(outfile, "out%.2d.txt", num);
    sprintf(errfile, "err%.2d.txt", num);

    if (access(infile, F_OK) == -1 && access(infile, R_OK) == -1){
        perror(infile);
        return -1;
    }

    freopen(infile, "r", stdin);
    freopen(outfile, "w", stdout);
    freopen(errfile, "w", stderr);

    return 0;
}

/* *
 * 具体测评函数
 * 使用fork，将待测评执行文件放入子进程，父进程监控
 * 父进程使用ptrace监控子进程
 * 获取内存的手段：
 *     读取/proc/pidd/statm文件的第6个数据，再 * getpagesize() / 1024，得到单位为B的内存大小
 * 获取内存的思路：
 *     1. 在源文件中添加文件读取代码（缺点，程序必须执行完成，但是若要求不高，则可试一试）
 *     2. 调试子进程，并不断读取文件
 */
int executor_function(int num){
    int status, memory = 0, current_time, answer;
    struct rusage info;
    bool isRight = false;

    if ((pid = fork()) == -1){
        perror("fork");
        return -1;
    }

    if (pid == 0){
        if (reopen(num) == -1){
            //kill自己
            kill(getpid(), SIGUSR1);
            exit(-1);
        }
        setlimits();
        
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);

        execl(executor_file_path.c_str(), executor_choise[style][1].c_str(), NULL);

        exit(0);

    }else {
        if (signal(SIGALRM, timer) == SIG_ERR){
            perror("set alarm");
            return -1;
        }

        alarm(limit_time);
        
        while(true){
            wait4(pid, &status, 0, &info);
            signal(SIGCHLD, SIG_IGN);

            //一些信号处理
            if (WIFEXITED(status)){
                //program exit
                if (WEXITSTATUS(status) == 0){
                    //exited normally
                    //虽然是正常退出，但是会存在AC WA PE三种情况
                    //下次用switch吧-_-|||
                    int ret = checkEachAnswer(num);
                    if (ret == -1){
                        answer = 100;
                    } else if (ret == 0){
                        //答案错
                        answer = 1;
                    } else {
                        if (isFormat){
                            ret = checkLineAnswer(num);
                            if (ret == -1){
                                //文件打开错误
                                answer = 100;
                            } else if (ret == 0){
                                //格式不正确
                                answer = 2;
                            } else{
                                //完全正确
                                answer = 0;
                            }
                        }else{
                            answer = 0;
                        }
                    }
                } else{
                    //another problem
                    answer = 100;
                }

                break;
            }else if (WIFSIGNALED(status)){
                int signo = WTERMSIG(status);
                if (signo == SIGKILL){
                    //over time
                    answer = 4;
                } else if (signo == SIGABRT){
                    //下列几乎没有用
                    answer = 100;
                    //cout << "abort" << endl;
                } else if (signo == SIGALRM){
                    answer = 100;
                    //cout << "alarm" << endl;
                } else {
                    //可能是没输入文件的错误
                    answer = 100;
                    //cout << "other signal " << signo << endl;
                }

                break;
            } else if (WIFSTOPPED(status)){
                int signo = WSTOPSIG(status);
                if (signo == SIGTRAP){
                    //程序中断点，用于获取内存大小
                    //直到退出前，都应该是此中断
                    ptrace(PTRACE_PEEKUSER, pid, NULL, NULL);
                    
                    FILE *fp;
                    char path[256];
                    sprintf(path, "/proc/%d/statm", pid);
                    if ((fp = fopen(path, "r")) != NULL){
                        for (int i = 0; i < 6; i++)
                            fscanf(fp, "%d", &memory);
                        fclose(fp);
                    }
                } else if (signo == SIGXFSZ) {
                    //file size
                    answer = 6;
                    break;
                } else if (signo == SIGSEGV) {
                    //虽然给了错误5，但是运行时似乎是抛出SIG_ABRT
                    answer = 5;
                    break;
                } else if (signo == SIGABRT) {
                    //memory
                    //runtime error
                    //cout << "stop abort" << endl;     //test
                    if (memory * psize / MB > (limit_memory - 1)){
                        answer = 5;
                    }else {
                        answer = 3;
                    }

                    break;
                } else {
                    answer = 100;
                    //cout << "signal other " << signo << endl;
                    break;
                }
            } else{
                //cout << "others" << endl;
                answer = 100;
                break;
            }

            ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        }
        current_time =  info.ru_utime.tv_sec * 1000 + info.ru_utime.tv_usec / 1000;
        if (answer == 4 && current_time < limit_time_origin){       //超时；测试里，每次少一点，补上，防止用户疑惑
            current_time = limit_time_origin + (info.ru_utime.tv_usec / 1000) % 100;
        }

        saveResult(isRight, answer, current_time, memory * psize);
    }

    return 0;
}

/* *
 * 参数分布情况：
 * extension: 待检测文件语言类型，现在分别映射为c java python，然后利用这个变量为style赋值 (1)
 * count: 测试文件总数 (2)
 * isFormat: 答案是否需要格式 (3)
 * limit_time_origin: 限制时间，可带小数点 (4, s)
 * limit_memory: 限制使用内存大小，不带小数点 (5, M)
 */
int main(int argc, char *argv[]){

    extension = argv[1];
    count = stoi(argv[2]);
    isFormat = (stoi(argv[3]) == 1 ? true : false);
    limit_time_origin = stof(argv[4]) * 1000;    //限制时间，放大到ms
    limit_memory = atoi(argv[5]);

    if (init()){
        cout << "executor init error" << endl;
        exit(-1);
    }

    for (int i = 0; i < count; i++){
        if (executor_function(i + 1)){
            cout << "executor_function error" << endl;
            exit(-1);
        }
    }

    return 0;
}
