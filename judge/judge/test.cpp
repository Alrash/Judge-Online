/*************************************************************************
	> File Name: test.cpp
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Tue 19 Jul 2016 02:01:16 PM CST
 ************************************************************************/

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <ctime>

using namespace std;

int main(){
    int pid = 0, status;
    for (int i = 0; i < 10; i++){
        pid = fork();
    }

    if (pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0){
    	int num = time(NULL) * 12541 % 64;
    	char command[56];
    	sprintf(command, "%.2d", num);
        execl("/home/alrash/Desktop/judge/judge/judge", "./judge", command, "22", "33", "44", "55", NULL);
        exit(0);
    }else{
        waitpid(pid, &status, 0);

        cout << "parent:" << getpid() << " child: " << pid << endl;
    }

    return 0;
}
