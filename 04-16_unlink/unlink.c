#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


int main(int argc, char* args[])
{
    int fd = open("./foo1", O_WRONLY|O_CREAT|O_TRUNC);

    /* unlink 将path文件的链接计数-1，当计数为0时由内核删除文件内容，
       如果有进程打开了文件，内容不能删除，
       关闭文件时，内核检查该文件打开的进程个数，为0时才检查链接计数 */

    unlink("./foo1");   // foo1被打开了

    close(fd);

    /*

     */
    
    exit(0);
}



