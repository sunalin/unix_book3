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


    fpin = fopen("./popen.c", "r");
    if (!fpin)
        perror("fopen1");
    

    /* popen(cmd,type)    创建管道，并且fork一个子进程，在子进程
       中关闭未使用的管道端并执行exec(cmd)，返回管道一端的IO文件指针

       "r": 返回的管道文件指针用于[读]，管道另一端连接子进程的[标准输出]
       "w": 返回的管道文件指针用于[写]，管道另一端连接子进程的[标准输入] */
       
    fpout = popen("${PAGER:-more}", "w");   /* 从环境变量里找PAGER,没找到则使用默认值more */
    if (!fpout)
        perror("fopen2");

    while (fgets(line, sizeof(line), fpin) != NULL)
        fputs(line, fpout); /* 写管道 */
    if (ferror(fpin))
        perror("ferror");

    pclose(fpout);      /* 关闭文件指针 */ 
    fclose(fpin);

    return 0;
}



