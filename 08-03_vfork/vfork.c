#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/resource.h>


int glob = 5;
char buf[] = "a write to stdout\r\n";

int main(int argc, char* args[])
{
    int var = 10;
    pid_t pid;

    write(STDOUT_FILENO, buf, sizeof(buf) - 1);

    printf("vfork befor[getpid = %d]\r\n", getpid());
    fflush(stdout);
    pid = vfork();
    printf("vfork after[getpid = %d]\r\n", getpid());

    if (pid < 0)
        printf("fork error\r\n");
    else if (pid > 0)
    {
        // parent process
        //sleep(2);
    }
    else if (pid == 0)
    {
        /* 1.vfork()创建子进程，在调用exec()之前或exit()之前，
             子进程与父进程共享数据段（与fork()不同，fork要拷
             贝父进程的数据段,堆栈段）
           2.调用vfork()后，子进程先执行，父进程被挂起，直到
             子进程调用了exec或exit之后，父进程才执行。
           使用vfork(),但子进程不调用exit()或exec()，父进程一直挂起，导致死锁！*/
        
        // child process
        glob++; // 子进程修改变量,会影响到父进程,在exec/exit/_exit之前仍在父进程的地址空间运行,此时父进程已被挂起
        var++;
        exit(0);    // exec/exit/_exit
    }

    printf(" getpid = %d,"
           " getppid = %d,"
           " glob = %d,"
           " var = %d\r\n", getpid(), getppid(), glob, var);

    /*
     ./vfork
     ./vfork > temp.out;cat temp.out
     */

    exit(0);
}




