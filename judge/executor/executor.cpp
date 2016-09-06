/*************************************************************************
	> File Name: executor.cpp
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Sun 04 Sep 2016 01:15:42 PM CST
 ************************************************************************/

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <error.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/ptrace.h>
#include <signal.h>
#include "check.h"
#include "../public/config.h"
#include "../public/functions.h"
#include "../public/log.h"

/* *
 * 暂时没有检查格式的选项
 * 不能检查格式形式，需改数据库或者detail.ini文件
 */
std::vector<std::string> executor_parameter = {
    "-access_path", "-error_path", "-infile", "-outfile",
    "-time", "-memory", "-sid", "-output", "-errput",
    "-path", "-executor", "-n"
};
std::map<std::string, std::string> parameter_map = {};
char result[1025];
int limit_time_origin = 0, limit_time = 0;
int limit_memory = 0;
int pid = 0;
//缩放比例
const int psize = getpagesize() / 1024;

int setLimitTime(){
    if (!is_numeric(parameter_map["time"])){
        return -1;
    }
    
    limit_time_origin = stoi(parameter_map["time"]);
    int tm_t  = limit_time_origin / 1000;
    return (tm_t * 1000 == limit_time_origin) ? tm_t : (tm_t + 1);
}

int init(int argc, char* argv[]){
    int i = 1;

    for (; i < argc; i++){
        if (!is_find_in_vector(executor_parameter, argv[i])){
            break;
        }
        if (argv[i + 1][0] == '-'){
            parameter_map[argv[i] + 1] = "";
        }else {
            parameter_map[argv[i] + 1] = argv[i + 1];
            ++i;
        }
    }

    if (i < argc){
        if (!parameter_map["error_path"].empty()){
            Log log = Log(parameter_map["error_path"].c_str());
            char error_message[1024];
            sprintf(error_message, "paramter %s is not existed", argv[i]);
            log.append(error_message, std::string(parameter_map["sid"] + " call executor function").c_str());
            return -1;
        }
    }

    if (parameter_map.size() != executor_parameter.size()){
        if (!parameter_map["error_path"].empty()){
            Log log = Log(parameter_map["error_path"].c_str());
            std::string error_message = std::string("less parameter, and now parameter maps");
            for (auto item : parameter_map){
                error_message += std::string(" " + item.first + " " + item.second);
            }
            log.append(error_message.c_str(), std::string(parameter_map["sid"] + " call executor function").c_str());
        }
        return -1;
    }

    if ((limit_time = setLimitTime()) == -1){
        if (!parameter_map["error_path"].empty()){
            Log log = Log(parameter_map["error_path"].c_str());
            char error_message[1024];
            sprintf(error_message, "time be supported to use numeric value");
            log.append(error_message, std::string(parameter_map["sid"] + " call executor function").c_str());
        }
        return -1;
    }

    if (!is_numeric(parameter_map["memory"])){
        if (!parameter_map["error_path"].empty()){
	        Log log = Log(parameter_map["error_path"].c_str());
	        char error_message[1024];
	        sprintf(error_message, "memory needs using numeric value");
	        log.append(error_message, std::string(parameter_map["sid"] + " call executor function").c_str());
        }
        return -1;
    }
    limit_memory = stoi(parameter_map["memory"]);

    return 0;
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
        Log log = Log(parameter_map["error_path"].c_str());
        char error_message[1024];
        sprintf(error_message, "No.%s %s", parameter_map["n"].c_str(), strerror(errno));
        log.append(error_message, std::string(parameter_map["sid"] + " call executor function").c_str());
        exit(-1);
    }
    return 0;
}

/* *
 * 设置子进程的各种限制
 */
int setlimits(){
    setlimit(RLIMIT_CPU, limit_time);                        //限制CPU时间
    setlimit(RLIMIT_AS, limit_memory);                       //内存限制limit_memory M
    setlimit(RLIMIT_FSIZE,  max_outfile_size);               //输出文件最大1M
    setlimit(RLIMIT_NOFILE, 10);                            //打开文件数11
    return 0;
}

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
 * 重定向标准输入、输出、错误流
 * 备注：
 *       不可以没有文件intfile, outfile
 */
int reopen(){
    if (access(parameter_map["infile"].c_str(), R_OK) == -1 
         || access(parameter_map["outfile"].c_str(), R_OK) == -1){
        Log log = Log(parameter_map["error_path"].c_str());
        log.append("need input or output file", (parameter_map["sid"] + " call executor function").c_str());
        return -1;
    }
    freopen(parameter_map["infile"].c_str(), "r", stdin);
    freopen(parameter_map["output"].c_str(), "w", stdout);
    freopen(parameter_map["errput"].c_str(), "w", stderr);
    return 0;
}
/* *
 * 具体测评函数
 * 使用fork，将待测评执行文件放入子进程，父进程监控
 * 父进程使用ptrace监控子进程
 * 判别结果：根据数据中的排列顺序(使用config.h文件中已定义的常量)
 *     1-AC 2-WA 3-PE 4-RE 5-TLE 6-MLE 7-OLE 8-CE 9-others
 * 执行结果转存至result中
 * 获取内存的手段：
 *     读取/proc/pidd/statm文件的第6个数据，再 * getpagesize() / 1024，得到单位为B的内存大小
 * 获取内存的思路：
 *     1. 在源文件中添加文件读取代码（缺点，程序必须执行完成，但是若要求不高，则可试一试）
 *     2. 调试子进程，并不断读取文件
 */
int executor(){
    int status, memory = 0, current_time, answer;
    struct rusage info;

    if ((pid = fork()) == -1){
        //日志
        if (!parameter_map["error_path"].empty()){
            Log log = Log(parameter_map["error_path"].c_str());
            log.append("fork error", (parameter_map["sid"] + " call executor function").c_str());
        }
        return -1;
    }

    if (pid == 0){
	    auto vt = split(parameter_map["executor"], " ");
	    char **argv = (char **)malloc((vt.size() + 1) * sizeof(char *));
	    char (*ss)[1025] = new char[vt.size() + 1][1025];
	    for (int i = 0; i < vt.size(); i++){
	        strcpy(ss[i], vt[i].c_str());
	        argv[i] = ss[i];
	    }
	    argv[vt.size()] = (char *)0;

        if (reopen() == -1){
            //原reopen函数，含有一些限制，现已去除，所以用不到这里
	        if (!parameter_map["error_path"].empty()){
	            Log log = Log(parameter_map["error_path"].c_str());
                log.append("reopen function error", std::string(parameter_map["sid"] + " call executor function").c_str());
	        }
            //kill自己
            kill(getpid(), SIGUSR1);
            exit(-1);
        }
        setlimits();        
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        //execl(parameter_map["path"].c_str(), parameter_map["executor"].c_str(), NULL);
        execv(parameter_map["path"].c_str(), argv);

        //不造什么原因，调用失败
	    if (!parameter_map["error_path"].empty()){
	        Log log = Log(parameter_map["error_path"].c_str());
            log.append("call execl function error", std::string(parameter_map["sid"] + " call executor function").c_str());
        }
        free(argv);
        delete[] ss;

        exit(-1);

    }else {
        if (signal(SIGALRM, timer) == SIG_ERR){
	        if (!parameter_map["error_path"].empty()){
	            Log log = Log(parameter_map["error_path"].c_str());
                log.append(strerror(errno), std::string(parameter_map["sid"] + " call executor function").c_str());
	        }
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
                    int ret = checkAnswer(parameter_map["outfile"].c_str(), parameter_map["output"].c_str());
                    if (ret == -1){
                        answer = ANSWER_OTHERS;
                    } else if (ret == 1){
                        //答案错
                        answer = ANSWER_WA;
                    } else {
                        answer = ANSWER_AC;
                    }
                } else{
                    //another problem
                    answer = ANSWER_OTHERS;
                }

                break;
            }else if (WIFSIGNALED(status)){
                int signo = WTERMSIG(status);
                if (signo == SIGKILL){
                    //over time
                    answer = ANSWER_TLE;
                } else if (signo == SIGABRT){
                    //下列几乎没有用
                    answer = ANSWER_OTHERS;
                    //cout << "abort" << endl;
                } else if (signo == SIGALRM){
                    answer = ANSWER_OTHERS;
                    //cout << "alarm" << endl;
                } else {
                    //可能是没输入文件的错误
                    answer = ANSWER_OTHERS;
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
                    answer = ANSWER_OLE;
                    break;
                } else if (signo == SIGSEGV) {
                    //虽然给了错误5，但是运行时似乎是抛出SIG_ABRT
                    answer = ANSWER_MLE;
                    break;
                } else if (signo == SIGABRT) {
                    //memory
                    //runtime error
                    //cout << "stop abort" << endl;     //test
                    if (memory * psize / MB > (limit_memory - 1)){
                        answer = ANSWER_MLE;
                    }else {
                        answer = ANSWER_RE;
                    }

                    break;
                } else {
                    answer = ANSWER_OTHERS;
                    //cout << "signal other " << signo << endl;
                    break;
                }
            } else{
                //cout << "others" << endl;
                answer = ANSWER_OTHERS;
                break;
            }

            ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        }

        current_time =  info.ru_utime.tv_sec * 1000 + info.ru_utime.tv_usec / 1000;
        if (answer == ANSWER_TLE && current_time < limit_time_origin){       //超时；测试里，每次少一点，补上，防止用户疑惑
            current_time = limit_time_origin + (info.ru_utime.tv_usec / 1000) % 100;
        }

        sprintf(result, "%d;%d;%d", answer, current_time, memory * psize);
    }

    return 0;
}

int main(int argc, char* argv[]){
    if (init(argc, argv) == -1){
        return INIT_ERROR;
    }
    char message[1024];
    Log log = Log(parameter_map["access_path"].c_str());
    sprintf(message, "No.%s init finish, and stand by executor", parameter_map["n"].c_str());
    log.append(message, std::string(parameter_map["sid"] + " call executor function").c_str());
    
    executor();

    sprintf(message, "No.%s executor finish", parameter_map["n"].c_str());
    log.append(message, std::string(parameter_map["sid"] + " call executor function").c_str());

    printf("%s", result);

    return 0;
}
