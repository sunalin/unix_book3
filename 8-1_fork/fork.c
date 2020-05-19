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

    printf("fork befor[getpid = %d]\r\n", getpid());
    fflush(stdout); /* fork之前刷新I/O缓冲区,fork也会复制缓冲区 */
    pid = fork(); /* fork    创建子进程,fork返回两次(父子各一次),子进程获得父进程(数据空间/堆/栈)的副本且相互独立不共享,共享只读代码段 */
    printf("fork after[getpid = %d]\r\n", getpid());

    if (pid < 0)
        printf("fork error\r\n");
    else if (pid > 0)
    {
        // parent process
        sleep(2);
    }
    else if (pid == 0)
    {
        // child process
        glob++; // 子进程修改变量,不会影响到父进程
        var++;
    }

    printf(" getpid = %d,"
           " getppid = %d,"
           " glob = %d,"
           " var = %d\r\n", getpid(), getppid(), glob, var);

    /*
     ./fork
     ./fork > temp.out;cat temp.out
     */

    exit(0);
}



