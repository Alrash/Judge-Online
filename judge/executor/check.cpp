/*************************************************************************
	> File Name: check.cpp
	> Author: Alrash
	> Mail: kasukuikawai@gmail.com
	> Created Time: Sun 17 Jul 2016 08:54:47 PM CST
 ************************************************************************/

#include <cstdio>
#include <cstring>
#include <unistd.h>

int checkEachAnswer(int num){
    char comparefile[55], file[55];
    FILE *fap, *fdp;

    sprintf(comparefile, "output%.2d.txt", num);
    sprintf(file, "out%.2d.txt", num);

    //打开文件，若文件不存在，及文件不可读，均报错
    if ((fap = fopen(file, "r")) == NULL){
        printf("answer file: no such file or can not read\n");
        return -1;
    }
    if ((fdp = fopen(comparefile, "r")) == NULL){
        printf("normal file: no such file or can not read\n");
        return -1;
    }

    char answer[256], compare[256];
    int ret = 1;

    while (!feof(fdp)){
        if (fscanf(fdp, "%s", compare) == -1){
            //文件结尾有一个空行
            break;
        }

        //提早结束
        if (feof(fap) || fscanf(fap, "%s", answer) == -1){
            //关闭文件，返回错误
            fclose(fap);
            fclose(fdp);
            return 0;
        }

        //检测答案是否一致
        if (strlen(answer) != strlen(compare) || strcmp(answer, compare) != 0){
            fclose(fap);
            fclose(fdp);
            return 0;
        }
    }

    //防止带测试结果文件多输出
    if (!feof(fap) && fscanf(fap, "%s", answer) != -1){
        ret = 0;
    }

    fclose(fdp);
    fclose(fap);

    return ret;
}

/* *
 * 大部分操作一样，为什么不提供函数模块
 *     1. 易修改变化
 *     2. 懒-_-|||
 */
int checkLineAnswer(int num){
    char comparefile[55], file[55];
    FILE *fap, *fdp;
    const int STRLRN = 1024;

    sprintf(comparefile, "output%.2d.txt", num);
    sprintf(file, "out%.2d.txt", num);

    //打开文件，若文件不存在，及文件不可读，均报错
    if ((fap = fopen(file, "r")) == NULL){
        printf("answer file: no such file or can not read\n");
        return -1;
    }
    if ((fdp = fopen(comparefile, "r")) == NULL){
        printf("normal file: no such file or can not read\n");
        return -1;
    }

    char answer[STRLRN], compare[STRLRN];
    int ret = 1;

    while (!feof(fdp)){
        if (fgets(compare, STRLRN, fdp) == NULL){
            //文件结尾有一个空行
            if (!feof(fap) && fgets(answer, STRLRN, fap) == NULL)
                break;
            else
                ret = 0;
        }

        //提早结束
        if (feof(fap) || fgets(answer, STRLRN, fap) == NULL){
            //关闭文件，返回错误
            fclose(fap);
            fclose(fdp);
            return 0;
        }
        printf("%s%s", answer, compare);

        //检测答案是否一致
        if (strlen(answer) != strlen(compare) || strcmp(answer, compare) != 0){
            fclose(fap);
            fclose(fdp);
            return 0;
        }
    }

    //防止带测试结果文件多输出
    if (!feof(fap) && fgets(answer, STRLRN, fap) == NULL){
        ret = 0;
    }

    fclose(fdp);
    fclose(fap);

    return ret;
}
