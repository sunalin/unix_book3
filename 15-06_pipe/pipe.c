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
    pid_t pid;
    int fd[2];  // fd[0]--read  fd[1]--write
    
    /* pipe    创建管道，用于进程通信 */
    if (pipe(fd) < 0)
        perror("pipe");

    pid = fork();
    if (pid > 0)
    {
        /* parent process */
        char line[4096];
        FILE* fp;

        close(fd[0]);
        fp = fopen("./pipe.c", "r");
        if (!fp)
            perror("fopen");
        while (fgets(line, sizeof(line), fp) != NULL)
            write(fd[1], line, strlen(line));   /* 父进程写管道 */
        if (ferror(fp))
            perror("ferror");

        waitpid(pid, 0, 0);
        close(fd[1]);
        fclose(fp);
    }
    else if (pid == 0)
    {
        /* child process */        
        char* cmd;
        char* cmd_argc;

        close(fd[1]);
        if (fd[0] != STDIN_FILENO)
            dup2(fd[0], STDIN_FILENO);  /* 使管道成为子进程的标准输入 */
        close(fd[0]);

        cmd = getenv("PAGER");
        if (cmd == NULL)
            cmd = "/bin/more";  /* more从标准输入读 */
        cmd_argc = strrchr(cmd, '/');    /* 最后一个'/' */
        if (cmd_argc)
            cmd_argc++;
        else
            cmd_argc = cmd;

        /* /bin/more more */
        if (execl(cmd, cmd_argc, (char*)0) < 0)
            perror("execl");
    }

    return 0;
}



