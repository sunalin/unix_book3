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
    int fd[2];
    FILE* fp;

    int* pipe_read = &fd[0];
    int* pipe_write = &fd[1];

    if (argc != 2)
    {
        printf("usage: ./pipe_dup2 <filepath>\r\n");
        exit(0);
    }

    fp = fopen(args[1], "r");
    if (fp == NULL)
    {
        printf("fopen error\r\n");
        exit(0);
    }
    
    if (pipe(fd) < 0)
    {
        printf("pipe error\r\n");
        exit(0);
    }

    pid = fork();
    if (pid < 0)
    {
        printf("fork error\r\n");
        exit(0);
    }
    else if (pid > 0)
    {
        /* parent process */
        char line[4096];
        close(*pipe_read);
        while (fgets(line, sizeof(line), fp) != NULL)
            write(*pipe_write, line, strlen(line));
        if (ferror(fp))
        {
            printf("fgets error\r\n");
            exit(0);
        }
        close(*pipe_write);

        if (waitpid(pid, 0, 0) < 0)
            printf("waitpid error\r\n");
        exit(0);
    }
    else if (pid == 0)
    {
        /* child process */        
        char* pager;
        char* argc0;
        close(*pipe_write);
        if (*pipe_read != STDIN_FILENO)
            dup2(*pipe_read, STDIN_FILENO);
        close(*pipe_read);

        pager = getenv("PAGER");
        if (pager == NULL)
            pager = "/bin/more";
        argc0 = strrchr(pager, '/');
        if (argc0 != NULL)
            argc0++;
        else
            argc0 = pager;

        if (execl(pager, argc0, (char*)0) < 0)
            printf("execl error: %s\r\n", pager);
    }

    /*
       ./pipe_dup2 pipe_dup2.c
     */

    exit(0);
}



