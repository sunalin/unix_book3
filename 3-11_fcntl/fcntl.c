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
    if      (open_mode == O_RDONLY) printf("O_RDONLY");     // ֻ����
    else if (open_mode == O_WRONLY) printf("O_WRONLY");     // ֻд��
    else if (open_mode == O_RDWR)   printf("O_RDWR");       // ��д��
    else                            printf("unknown mode");

    printf(", ");
    
    if (val & O_APPEND)             printf("O_APPEND");     // дʱ��������ļ�β
    if (val & O_NONBLOCK)           printf("O_NONBLOCK");   // ��������ʽ
    if (val & O_SYNC)               printf("O_SYNC");       // �ȴ�д���
    if (val & O_ASYNC)              printf("O_ASYNC");      // �첽I/O
    
    printf("\r\n");

    /*
        1.  ��׼����   (stdin) ������Ϊ 0 ��ʹ�� < �� << �� /dev/stdin -> /proc/self/fd/0   0����/dev/stdin 
        2.  ��׼���   (stdout)������Ϊ 1 ��ʹ�� > �� >> �� /dev/stdout -> /proc/self/fd/1  1����/dev/stdout
        3.  ��׼�������(stderr)������Ϊ 2 ��ʹ�� 2> �� 2>> �� /dev/stderr -> /proc/self/fd/2 2����/dev/stderr

        
        ./fcntl 0 0</dev/tty
        ./fcntl 1 1>/dev/tty
        ./fcntl 1 1>>/dev/tty
        ./fcntl 2 2>>/dev/tty
        ./fcntl 5 5<>/dev/tty
     */
    
    return 0;
}



