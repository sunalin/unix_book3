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

    pid = fork();
    if (pid < 0)
    {
        printf("fork error\r\n");
        exit(0);
    }
    else if (pid > 0)
    {
        /* parent process */
        exit(0);
    }

    /* child process */
    setsid();
    chdir("/");
    umask(0);

    return 0;
}



