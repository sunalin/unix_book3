#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


int main(int argc, char* args[])
{
    /* stat fstat lstat 打印文件类型 */

    struct stat st;
    char* ptr;

    for (int i = 0; i < argc; i++)
    {
        printf("[%s]:", args[i]);
        if (lstat(args[i], &st) < 0)
        {
            printf("lstat error\r\n");
            continue;
        }

        if      (S_ISREG(st.st_mode))   printf("S_ISREG  \r\n");    // 普通文件
        else if (S_ISDIR(st.st_mode))   printf("S_ISDIR  \r\n");    // 目录文件
        else if (S_ISCHR(st.st_mode))   printf("S_ISCHR  \r\n");    // 字符设备文件
        else if (S_ISBLK(st.st_mode))   printf("S_ISBLK  \r\n");    // 块设备文件
        else if (S_ISFIFO(st.st_mode))  printf("S_ISFIFO \r\n");    // 管道或FIFO
        else if (S_ISLNK(st.st_mode))   printf("S_ISLNK  \r\n");    // 符号链接
        else if (S_ISSOCK(st.st_mode))  printf("S_ISSOCK \r\n");    // 套接字
    }
    /*
        ./stat /vmlinuz /etc /dev/tty /dev/sda /var/spool/cron/FIFO /bin /dev/printer
     */
    
    return 0;
}



