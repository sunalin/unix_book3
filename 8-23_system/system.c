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

void pr_exit(int status)
{
    if (WIFEXITED(status))
        printf("child process normal termination, [exit(status)] status = %d\r\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("child process abnormal termination, signal number = %d%s\r\n", WTERMSIG(status), WCOREDUMP(status) ? "(core file.generated)" : "");
    else if (WIFSTOPPED(status))
        printf("child process stopped, signal number = %d\r\n", WSTOPSIG(status));
    else
        printf("pr_exit error\r\n");
}

int main(int argc, char* args[])
{
    int status = -1;

    /* system    可相当于在shell执行命令，system(fork >> exec >> /bin/sh) */
    
    printf("==================================\r\n");
    status = system("date");
    pr_exit(status);

    printf("==================================\r\n");
    status = system("ls -a");
    pr_exit(status);

    printf("==================================\r\n");
    status = system("pwd");
    pr_exit(status);

    /*
        ./system
     */

    exit(0);
}



