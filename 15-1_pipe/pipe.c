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
    char buf[4096];

    int* pipe_read = &fd[0];
    int* pipe_write = &fd[1];

    /* parent ==>> child */
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
        close(*pipe_read);
        write(*pipe_write, "hello world\r\n", 13);
    }
    else if (pid == 0)
    {
        /* child process */
        int n;
        close(*pipe_write);
        n = read(*pipe_read, buf, sizeof(buf));
        write(STDOUT_FILENO, buf, n);
    }

    exit(0);
}



