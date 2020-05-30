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
    int fdin;
    int fdout;
    char* src;
    char* dst;
    struct stat sbuf;


    fdin  = open("./Makefile", O_RDONLY);
    fdout = open("./temp.out", O_RDWR|O_CREAT|O_TRUNC);
    if (fdin < 0 || fdout < 0)
        perror("open");

    fstat(fdin, &sbuf);                     /* 获取原文件长度 */
    //lseek(fdout, sbuf.st_size, SEEK_SET);   /* 设置输出文件长度 */
    ftruncate(fdout, sbuf.st_size);         /* 设置输出文件长度 */


    /* mmap(aar,len,port,flag,fd,off)    可将磁盘文件映射到一个缓冲区上，
       对缓存执行读写就相当于对文件执行读写，这样不用read/write也可以执行IO读写

       addr:映射缓冲区首地址，为0时由系统指定，通过返回值返回，映射之后可通过该地址操作
       len: 映射缓冲区长度
       prot:指定读写(要与fd的属性相对应)
       flag: MAP_SHARED(直接操作文件) MAP_PRIVATE(操作文件的副本)
       fd:  文件描述符
       off: 文件偏移量(文件从此偏移量开始映射len长度) */

    src = mmap(0, sbuf.st_size, PROT_READ,            MAP_SHARED, fdin,  0);
    dst = mmap(0, sbuf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fdout, 0);
    if (!src || !dst)
        perror("mmap");

    memcpy(dst, src, sbuf.st_size); /* 对缓存执行读写就相当于对文件执行读写 */    
    msync(src, sbuf.st_size, MS_SYNC);   /* 刷新映射到文件 */ //fsync(fd)
    msync(dst, sbuf.st_size, MS_SYNC);   /* 刷新映射到文件 */ //fsync(fd)
    printf("copy success...\r\n");

    munmap(src, sbuf.st_size);   /* 解除映射，MAP_PRIVATE的文件将被丢弃 */
    munmap(dst, sbuf.st_size);   /* 解除映射，MAP_PRIVATE的文件将被丢弃 */
    close(fdin);
    close(fdout);

    /* 以下是打印部分 */
    printf("===============================================\r\n");
    system("cat Makefile");
    printf("===============================================\r\n");
    system("cat temp.out");
    system("rm temp.out");

    exit(0);
}



