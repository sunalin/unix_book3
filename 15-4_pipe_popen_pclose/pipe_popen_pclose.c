#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <wait.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>
#include <time.h>

#include <stropts.h>
#include <sys/mman.h>

int main(int argc, char* args[])
{
    char line[8192];
    FILE* fpin;
    FILE* fpout;

    if (argc != 2)
    {
        printf("usage: ./pipe_popen_pclose <filepath>\r\n");
        exit(0);
    }

    fpin = fopen(args[1], "r");
    if (fpin == 0)
    {
        printf("fopen error\r\n");
        exit(0);
    }

    /*
       popen 会fork一个子进程,返回pipe的一个FILE*流指针
       "r":该流 相当于子进程的标准输出
       "w":该流 相当于子进程的标准输入
     */
    fpout = popen("${PAGER:-more}", "w");   // 从环境变量里找PAGER,没找到则使用默认值more
    if (fpout == 0)
    {
        printf("popen error\r\n");
        exit(0);
    }

    while (fgets(line, sizeof(line), fpin) != NULL)
        fputs(line, fpout);
    if (ferror(fpin))
    {
        printf("fgets error\r\n");
        exit(0);
    }

    pclose(fpout);

    /*
       ./pipe_popen_pclose pipe_popen_pclose.c
     */

    exit(0);
}



