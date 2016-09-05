/*************************************************************************
	> File Name: check.cpp
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Sun 17 Jul 2016 08:54:47 PM CST
 ************************************************************************/

#include <cstdio>
#include <cstring>
#include <unistd.h>

int checkAnswer(const char* outfile, const char* output){
    FILE *file, *origin;
    char f1[1024], f2[1024];
    char command[1025] = "cat %s | tr -d '\\t \\n\\r' | sha256sum | cut -d ' ' -f1";
    
    sprintf(f1, command, outfile);
    sprintf(f2, command, output);
    
    if ((file = popen(f1, "r")) == nullptr){
        return -1;
    }
    if ((origin = popen(f2, "r")) == nullptr){
        return -1;
    }

    char standard[100], answer[100];
    memset(standard, 0, sizeof(standard));
    memset(answer, 0, sizeof(answer));

    fgets(standard, 99, origin);
    fgets(answer, 99, file);

    pclose(file);
    pclose(origin);

    if (strcmp(standard, answer))
        return 1;
    else
        return 0;
}
