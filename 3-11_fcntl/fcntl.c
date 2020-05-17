#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


void set_fl(int fd, int flags)
{
    int val;

    val = fcntl(fd, F_GETFL, 0);
    if (val < 0)
        printf("fcntl F_GETFL error\r\n");

    // turn on flags
    val |= flags;

    if (fcntl(fd, F_SETFL, val) < 0)
        printf("fcntl F_SETFL error\r\n");
}

void clr_fl(int fd, int flags)
{
    int val;

    val = fcntl(fd, F_GETFL, 0);
    if (val < 0)
        printf("fcntl F_GETFL error\r\n");

    // turn flags off
    val &= ~flags;

    if (fcntl(fd, F_SETFL, val) < 0)
        printf("fcntl F_SETFL error\r\n");
}

int main(int argc, char *args[])
{
    int open_mode;
    int val;

    //set_fl(STDOUT_FILENO, O_SYNC);
    if (argc != 2)
    {
        printf("usage: cmd params\r\n");
        return 0;
    }

    for (int i = 0; i < argc; i++)
        printf("argc[%d]:%s\r\n", i, args[i]);

    val = fcntl(atoi(args[1]), F_GETFL, 0);
    if (val < 0)
    {
        printf("fcntl error\r\n");
        return 0;
    }

    open_mode = val & O_ACCMODE;
    if      (open_mode == O_RDONLY) printf("O_RDONLY");     // 只读打开
    else if (open_mode == O_WRONLY) printf("O_WRONLY");     // 只写打开
    else if (open_mode == O_RDWR)   printf("O_RDWR");       // 读写打开
    else                            printf("unknown mode");

    printf(", ");
    
    if (val & O_APPEND)             printf("O_APPEND");     // 写时都添加至文件尾
    if (val & O_NONBLOCK)           printf("O_NONBLOCK");   // 非阻塞方式
    if (val & O_SYNC)               printf("O_SYNC");       // 等待写完成
    if (val & O_ASYNC)              printf("O_ASYNC");      // 异步I/O
    
    printf("\r\n");

    /*
        1.  标准输入   (stdin) ：代码为 0 ，使用 < 或 << ； /dev/stdin -> /proc/self/fd/0   0代表：/dev/stdin 
        2.  标准输出   (stdout)：代码为 1 ，使用 > 或 >> ； /dev/stdout -> /proc/self/fd/1  1代表：/dev/stdout
        3.  标准错误输出(stderr)：代码为 2 ，使用 2> 或 2>> ； /dev/stderr -> /proc/self/fd/2 2代表：/dev/stderr

        
        ./fcntl 0 0</dev/tty
        ./fcntl 1 1>/dev/tty
        ./fcntl 1 1>>/dev/tty
        ./fcntl 2 2>>/dev/tty
        ./fcntl 5 5<>/dev/tty
     */
    
    return 0;
}



