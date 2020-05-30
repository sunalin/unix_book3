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
        close(fd[0]);
        write(fd[1], "hello world\r\n", 13);    /* 父进程写管道 */
    }
    else if (pid == 0)
    {
        /* child process */
        int n;        
        char buf[100];
        close(fd[1]);
        n = read(fd[0], buf, sizeof(buf));      /* 子进程读管道 */
        write(STDOUT_FILENO, buf, n);
    }
    sleep(1);

    return 0;
}



