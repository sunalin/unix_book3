#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <wait.h>


int main(int argc, char* args[])
{
    char* env_paths[] = {"USER=sunalin", "PATH=/tmp", NULL};
    pid_t pid;


    printf("=======================================\r\n");
    pid = fork();
    if (pid > 0)
    {
        // parent process
        waitpid(pid, 0, 0);
    }
    else if (pid == 0)
    {
        // child process
        // 从环变量查找文件名为ls的程序
        if (execlp("ls", "ls", "-a", (char*)0) < 0)
            printf("execlp error\r\n");
    }

    printf("=======================================\r\n");
    pid = fork();
    if (pid > 0)
    {
        // parent process
        waitpid(pid, 0, 0);
    }
    else if (pid == 0)
    {
        // child process
        // environ.c: 打印所有命令行参数 打印所有环境变量
        if (execle("./environ", "environ", "argc1", (char*)0, env_paths) < 0)
            printf("execle error\r\n");
    }

    printf("=======================================\r\n");
    pid = fork();
    if (pid > 0)
    {
        // parent process
        waitpid(pid, 0, 0);
    }
    else if (pid == 0)
    {
        // child process
        /* 执行解释器文件(最好在命令行下创件该文件,文件要有可执行属性 chmod 777 environ.sh)
           请注意UNIX的换行符是\n,使用git拉取还原脚本文件时需注意

           exec传入的命令行参数 会被追加在 解释器文件中的可选参数后面
           注:exec的pathname会代替第一个命令行参数(下例中即"argc0"被替换成"./environ.sh") */
        if (execle("./environ.sh", "argc0", "argc1", (char*)0, env_paths) < 0)
            printf("execle error\r\n");
    }

    printf("=======================================\r\n");
    pid = fork();
    if (pid > 0)
    {
        // parent process
        waitpid(pid, 0, 0);
    }
    else if (pid == 0)
    {
        // child process
        if (execl("./bash.sh", "argc0", "argc1", (char*)0) < 0)
            printf("execl error\r\n");
    }
    
    printf("=======================================\r\n");
    pid = fork();
    if (pid > 0)
    {
        // parent process
        waitpid(pid, 0, 0);
    }
    else if (pid == 0)
    {
        // child process
        if (execl("./awk.sh", "argc0", "argc1", (char*)0) < 0)
            printf("execl error\r\n");
    }


    /*
       ./exec
     */

    exit(0);
}



