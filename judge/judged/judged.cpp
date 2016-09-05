/*************************************************************************
	> File Name: judge.cpp
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Thu 01 Sep 2016 03:53:00 PM CST
 ************************************************************************/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include "MySQLDB.h"
#include "../public/readConfig.h"
#include "../public/log.h"
#include "../public/config.h"
#include "../public/functions.h"

#include <iostream>
using namespace std;

#define PATH_LENGTH 1024
#define QUESTION_FILE "Questions"
#define SUBMISSION_FILE "Submit"

/* *
 * all 存放所有的配置信息
 * access_path 可通过日志路径
 * error_path 错误日志路径
 * parameter_map 存放从judge传过来的参数
 * executor_map 存放运行时，需要的系统程序路径与运行指令
 *     (execl 使用，如execl("/usr/bin/java", "java", "Main.class") 之类)
 */
std::map<std::string, std::map<std::string, std::string>> all;
char access_path[PATH_LENGTH + 1], error_path[PATH_LENGTH + 1];
std::map<std::string, std::string> parameter_map = {};
std::map<std::string, std::vector<std::string>> executor_map = {};
int open_mode = O_RDONLY;
int pipe_id = -1;
bool isError = false;
char compiler_path[PATH_LENGTH + 1], executor_path[PATH_LENGTH + 1];

bool has_group(std::string group_name){
    if (all.find(group_name) == all.end())
        return false;
    return true;
}

bool has_key(std::string group_name, std::string key){
    if (has_group(group_name) 
        && all[group_name].find(key) != all[group_name].end())
        return true;
    return false;
}

void showConfigForTest(){
    for (auto item : all){
        std::cout << item.first << std::endl;
        for (auto key : item.second){
            std::cout << "\t" << key.first << "\t" << key.second << std::endl;
        }
    }
}

std::string getPath(std::string path, std::string filename){
    if (path.empty()){
        return filename;
    }else{
        return std::string(path + "/" + filename);
    }
}

bool has_or_create_file(const char* path){
    if (access(path, W_OK) == -1){
        int file = open(path, O_RDWR | O_CREAT, 0744);
        if (file != -1){
            close(file);
        }else{
            fprintf(stderr, "please check path %s or use root permission\n", path);
            exit(EXIT_FAILURE);
        }
    }
    return true;
}

/* *
 * 创建path路径上的所有文件夹
 * 前提是path路径正确(未测试/var/log//judge这样的情况)
 * 未使用递归，直接进行拆分
 */
void mkdir_p(std::string path, const char* function_name){
    std::vector<std::string> p;

    path = "/" + path;
    while(opendir(path.c_str()) == nullptr){
        p.push_back(path.substr(1));
        path = path.substr(0, path.find_last_of("/"));
    }

    std::reverse(p.begin(), p.end());
    for (auto item : p){
        if (mkdir(item.c_str(), 0755) == -1){
            char error_message[1024];
            sprintf(error_message, "[%s] cannot create/open directory %s", function_name, item.c_str());
            perror(error_message);
            exit(EXIT_FAILURE);
        }
    }
}

int init(){
    bool hasLog = false;

    ReadConfig config;
    for (auto item : config.getGroups()){
        all[item] = config.getGroupMaps(item.c_str());
    }

    //showConfigForTest();

    /* *
     * 创建pipe管道文件
     * 检测管道是否存在，并且可以读取
     * 注意：这里仅仅检测读取问题，其余问题，如http用户不能写入，没有编入
     * 之后以只读方式打开管道
     */
    if (!has_key("pipe", "path") || all["pipe"]["path"].empty()){
        fprintf(stderr, "please set [pipe] path value\n");
        exit(EXIT_FAILURE);
    }else {
        if (access(all["pipe"]["path"].c_str(), F_OK) == -1){
            //管道不存在，创建
            //更改掩码值
            mode_t system_umask = umask(0000);
            //检查路径是否存在
            mkdir_p(all["pipe"]["path"].substr(0, all["pipe"]["path"].find_last_of("/")), "pipe path");
            /* *
             * 关于问题权限的问题：
             * 暂时不会解决，自动创建用户与用户组，以及识别之的问题，本程序pipe给予最宽松的权限rw_rw_rw_
             */
            if (mkfifo(all["pipe"]["path"].c_str(), 0666) != 0){
                //不能创建文件，退出
                fprintf(stderr, "can not create fifo %s: Permission denied\n", all["pipe"]["path"].c_str());
                exit(EXIT_FAILURE);                                
            }
            //还原掩码值
            umask(system_umask);
        }else {
            if (access(all["pipe"]["path"].c_str(), R_OK) == -1){
                //管道不可读
                perror(all["pipe"]["path"].c_str());
                exit(EXIT_FAILURE);
            }
        }
    }
    
    /* *
     * 切换工作目录，至[path] judge下
     * 若无配置，或者不能切换，均退出程序
     */
    if (!has_key("path", "judge")){
        fprintf(stderr, "请在配置文件中设置[path] judge值");
        exit(EXIT_FAILURE);
    }
    if (chdir(all["path"]["judge"].c_str()) == -1){
        char error_message[1024];
        sprintf(error_message, "[init] change directory %s", all["path"]["judge"].c_str());
        perror(error_message);
        exit(EXIT_FAILURE);
    }
    sprintf(compiler_path, "%s/compiler", all["path"]["bin"].c_str());
    sprintf(executor_path, "%s/executor", all["path"]["bin"].c_str());

    /* *
     * executor_map用
     */
    if (!has_group("executor")){
        fprintf(stderr, "请在配置文件中设置executor组");
        exit(EXIT_FAILURE);
    }
    for (auto item : all["executor"]){
        executor_map[item.first] = split(item.second, ";");
        if (!all["extension"][item.first].compare("cpp") || !all["extension"][item.first].compare("c")){
            executor_map[item.first][0] = replaceAll(executor_map[item.first][0], "{check_path}", all["path"]["judge"]);
        }
    }

    /* *
     * 日志文件设置
     * 获取文件位置，写入access_path error_path
     * 配置文件中，可以缺省path路径
     * 缺省时，access 与 error默认放入[path] judge下，
     *     但是不建议这样做，因为之后会删除整个judge，再重新放入check文件
     * 也可缺省access error
     *     缺省时，不使用文件
     */
    if (has_key("log", "path") && !all["log"]["path"].empty()){
        //查看路径是否存在
        if (opendir(all["log"]["path"].c_str()) == nullptr){
            //文件夹不存在，创建文件夹(所有路径)
            mkdir_p(all["log"]["path"], "log_file");
        }
    }
    if (has_key("log", "access")){
        hasLog = true;
        strncpy(access_path, getPath(all["log"]["path"], all["log"]["access"]).c_str(), PATH_LENGTH);
        has_or_create_file(access_path);
    }else{
        strcpy(access_path, "");
    }
    if (has_key("log", "error")){
        hasLog = true;
        strncpy(error_path, getPath(all["log"]["path"], all["log"]["error"]).c_str(), PATH_LENGTH);
        has_or_create_file(error_path);
    }else{
        strcpy(error_path, "");
    }

    if (hasLog == false){
        fprintf(stdout, "Note:do not use log function!\n");
    }

    /* *
     * 擦除大部分配置信息，释放内存
     * need为保留的配置组
     */
    /*std::vector<std::string> need = {
        "compiler", "answer", "pipe", "script"
    };*/

    return 0;
}

int open_pipe(){
    int id = -1;
    if ((id = open(all["pipe"]["path"].c_str(), open_mode)) == -1){
        //管道不能打开
        char error_message[1024];
        sprintf(error_message, "cannot open fifo %s", all["pipe"]["path"].c_str());
        perror(error_message);
        return -1;
    }
    return id;
}

/* *
 * 获得参数，并且赋值给parameter_map
 * 如果参数不存在，或者参数值形式不正确，返回-1，并记录信息
 * @return
 *     0/-1 同部分系统函数的定义
 */
int getInfo(std::string info){
    std::string substr = std::string("") + fillch;
    std::vector<std::string> tmp;
    
    info = replaceAll(info, substr, "");
    std::vector<std::string> subInfo = split(info, std::string(std::string() + splitch));

    for (auto item : subInfo){
        tmp = split(item, " ");
        if (!is_find_in_vector(parameter, tmp[0]) && !std::string(error_path).empty()){
            Log log = Log(error_path);
            log.append(std::string("information " + info + " has error within some parameters").c_str(), "judged getInfo");
            return -1;
        }

        if (!tmp[0].compare("-l")){
            if (all["compiler"].find(tmp[1]) == all["compiler"].end() && !std::string(error_path).empty()){
                Log log = Log(error_path);
                log.append(std::string("information " + info + " has not compiler choise").c_str(), "judged getInfo");
                return -1;
            }
            parameter_map["compiler"] = all["compiler"][tmp[1]];
            parameter_map["-l"] = tmp[1];
        }else {
            if (!is_numeric(tmp[1]) && !std::string(error_path).empty()){
                Log log = Log(error_path);
                log.append(std::string("information " + info + " the value of parameter " + tmp[0] + " is not numeric").c_str(), "judged getInfo");
                return -1;
            }
            parameter_map[tmp[0]] = tmp[1];
        }
    }

    parameter_map["-m"] = to_string(stoi(parameter_map["-m"]) * MB);

    return 0;
}

void ready(){
    char command[1024];
    if (!std::string(access_path).empty()){
        Log log = Log(access_path);
        sprintf(command, "be ready to cp %s in/out file directory to root folder", parameter_map["-p"].c_str());
        log.append(command, parameter_map["-s"].c_str());
    }

    //复制测试文件内容
    sprintf(command, "cp %s/%s/%s/in/* .", all["path"]["root"].c_str(), QUESTION_FILE, parameter_map["-p"].c_str());
    system(command);
    sprintf(command, "cp %s/%s/%s/out/* .", all["path"]["root"].c_str(), QUESTION_FILE, parameter_map["-p"].c_str());
    system(command);
    //复制源文件
    sprintf(command, "cp %s/%s/%s/%s.%s temp.%s", 
            all["path"]["root"].c_str(), SUBMISSION_FILE, parameter_map["-s"].c_str(), parameter_map["-s"].c_str(),
            all["extension"][parameter_map["-l"]].c_str(), all["extension"][parameter_map["-l"]].c_str());
    system(command);
}

void createGoalIni(std::vector<std::string> goal, std::vector<std::string> run_time, std::vector<std::string> run_memory, int count){
    char ini_path[1024];
    FILE *fp;

    sprintf(ini_path, "%s/Submit/%s/goal.ini",  all["path"]["root"].c_str(), parameter_map["-s"].c_str());
    if ((fp = fopen(ini_path, "w")) == NULL){
        Log log = Log(error_path);
        log.append("cannot write goal.ini file", parameter_map["-s"].c_str());
        return;
    }

    for (int i = 1; i <= count; i++){
        fprintf(fp, "[%d]\n", i);
        fprintf(fp, "goal = %.02f\n", std::stof(goal.at(i - 1)));
        fprintf(fp, "runtime = %s\n", run_time.at(i - 1).c_str());
        fprintf(fp, "runmemory = %s\n", run_memory.at(i - 1).c_str());
    }

    fclose(fp);
}

void updateDatabase(std::string sid, std::string uid, std::string pid, const std::vector<std::string> &result, int count){
    auto goal = std::vector<std::string>();
    auto run_time = std::vector<std::string>();
    auto run_memory = std::vector<std::string>();
    auto answer = std::vector<std::string>();
    float each = 100.00 / count;
    int right = 0;

    for (int i = 0; i < count; i++){
        if (result.empty()){
            //编译时，错误
            goal.push_back("0");
            answer.push_back(answer_map.at(std::to_string(ANSWER_CE)));
            run_time.push_back("0");
            run_memory.push_back("0");
        }else {
            auto vt = split(result[i], ";");
            if (std::stoi(vt[0]) == ANSWER_AC){
                goal.push_back(std::to_string(each));
                right++;
            }else {
                goal.push_back("0");
            }
            answer.push_back(answer_map.at(vt[0]));
            run_time.push_back(vt[1]);
            run_memory.push_back(vt[2]);
        }
    }

    createGoalIni(goal, run_time, run_memory, count);

    /* *
     * 数据库的更新有三个地方：submission questionstatistics userstatistics
     * submission: goal 总 status 第一个不是正确的 time memory 同左
     * questionstatistics: right ac + 1 wrong others + 1
     * userstatistics: 1-8 right == count ac + 1, others 同submission status
     */
    int i = 0;
    char sql[1025];
    MySQLDB db;

    db.init();
    while (i < count){
        if (std::stoi(result.at(i)) != ANSWER_AC){
            break;
        }
        i++;
    }
    i = i == count ? 0 : i;
    int status = std::stoi(result.at(i));
    sprintf(sql, "update `Submission` set `status` = %d, `Runtime` = %s, `Runmemory` = %s, `goal` = %.2f where `SId` = %s",
            status, run_time[i].c_str(), run_memory[i].c_str(), 
            (right == count) ? 100 : (right * each), parameter_map["-s"].c_str());
    db.update(sql);
    sprintf(sql, "update `QuestionStatistics` set `%s` = `%s` + 1 where `PId` = %s", 
            (right == count) ? "Right" : "Wrong", (right == count) ? "Right" : "Wrong",
            parameter_map["-p"].c_str());
    db.update(sql);
    sprintf(sql, "update `UserStatistics` set `%s` = `%s` + 1 where `UId` = %s",
            answer_map.at(std::to_string(status)).c_str(), 
            answer_map.at(std::to_string(status)).c_str(), parameter_map["-u"].c_str());
    db.update(sql);
    db.close();
}

int running(){
    //这里的pid为进程号，而非问题号
    int pid = 0;
    int status;
    char command[2049];
    int count = stoi(parameter_map["-c"]);
    auto result = std::vector<std::string>();
    
    sprintf(command, "%s -access_path %s -error_path %s -compiler \"%s\" -extension %s -script \"%s\" -sid %s",
           compiler_path, access_path, error_path, parameter_map["compiler"].c_str(),
           all["extension"][parameter_map["-l"]].c_str(), all["script"][parameter_map["-l"]].c_str(),
           parameter_map["-s"].c_str());
    status = system(command);
    if (WIFEXITED(status)){
        //编译程序正常退出
        int return_code = WEXITSTATUS(status);
        if (return_code == INIT_ERROR){
            //compiler初始化错误，可查看error_path下的文件
            return -1;
        }else if (return_code == COMPILER_ERROR){
            //编译脚本或源程序本身的问题
            updateDatabase(parameter_map["-s"], parameter_map["-u"], parameter_map["-p"], result, count);
            return -1;
        }
    }else {
        //退出异常
        Log log = Log(error_path);
        log.append("unkown error", std::string(parameter_map["-s"] + "when call compiler function").c_str());
        return -1;
    }

	for (int i = 1; i <= count; i++){
	    sprintf(command, "%s -access_path %s -error_path %s -path %s -executor %s -infile input%.2d.txt -outfile output%.2d.txt -output out%0.2d.txt -errput err%0.2d.txt -time %s -memory %s -sid %s -n %d",
	           executor_path, access_path, error_path, executor_map[parameter_map["-l"]][0].c_str(),
	           executor_map[parameter_map["-l"]][1].c_str(), i, i, i, i, parameter_map["-t"].c_str(),
	           parameter_map["-m"].c_str(), parameter_map["-s"].c_str(), i);
        result.push_back(std::string());
	    FILE *fp;
	    if ((fp = popen(command, "r")) == nullptr){
	        //执行失败
            Log log = Log(error_path);
            log.append("unkown error", std::string(parameter_map["-s"] + "when call executor function").c_str());
	    }else {
	        char info[1025];
            memset(info, 0, sizeof(info));
	        fgets(info, 1024, fp);
	        pclose(fp);
	
	        if (!std::string(info).empty()){
	            //测试成功；失败一般已经记录
                result.back() = info;
	        }
	    }
    }

    updateDatabase(parameter_map["-s"], parameter_map["-u"], parameter_map["-p"], result, count);
    return 0;
}

void clear(){
    char command[1025];
    sprintf(command, "cp err* %s/%s/%s", all["path"]["root"].c_str(), SUBMISSION_FILE,  parameter_map["-s"].c_str());
    system(command);

    system("rm -rf *");
    if (!std::string(access_path).empty()){
        Log log = Log(access_path);
        char message[1024];
        sprintf(message, "testing finish, and clear up all tempreture file");
        log.append(message, parameter_map["-s"].c_str());
    }
}

int main(){
    init();

    daemon(1, 1);
    //打开管道文件
    //貌似用(?, 0)输出不了-_-|||
    if ((pipe_id = open_pipe()) == -1){
        exit(EXIT_FAILURE);
    }

    char content[PIPE_LENGTH + 1];
    int ret = 0;
    while(true){
        ret = read(pipe_id, content, PIPE_LENGTH);
        content[PIPE_LENGTH] = '\0';
        
        if (ret == -1){
            continue;
        }
        
        if (ret == 0){
            //重新开启pipe，防止
            close(pipe_id);
            if ((pipe_id = open_pipe()) == -1){
                Log log = Log(error_path);
                log.append(std::string("cannot reopen " + all["pipe"]["path"]).c_str(), "judged while loop");
                exit(EXIT_FAILURE);
            }
            continue;
        }

        if (getInfo(content) == -1){
            continue;
        }

        ready();
        running();
        clear();
    }

    return 0;
}
