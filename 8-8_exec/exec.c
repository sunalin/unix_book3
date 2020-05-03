#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <wait.h>


char* env_paths[] = {"USER=sunalin", "PATH=/tmp", NULL};

int main(int argc, char* args[])
{
    pid_t pid;

    /*=====================================*/
    pid = fork();
    if (pid < 0)
        printf("fork error\r\n");
    else if (pid > 0)
    {
        // parent process
        if (waitpid(pid, 0, 0) < 0)
            printf("waitpid error\r\n");
    }
    else if (pid == 0)
    {
        // child process
        // 7-2_environ: 打印所有命令行参数 打印所有环境变量
        if (execle("./environ",
            "environ", "my_arg1", "my Arg2", "aa3", (char*)0,
            env_paths) < 0)
            printf("execle error\r\n");
    }

    /*=====================================*/
    pid = fork();
    if (pid < 0)
        printf("fork error\r\n");
    else if (pid > 0)
    {
        // parent process
        if (waitpid(pid, 0, 0) < 0)
            printf("waitpid error\r\n");
    }
    else if (pid == 0)
    {
        // child process
        if (execlp("ls", "ls", "-hal", (char*)0) < 0)
            printf("execlp error\r\n");
    }
    exit(0);
}



