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
#include <pthread.h>    // -lpthread
#include <aio.h>        // -lrt


int main(int argc, char* args[])
{
    struct aiocb aio;
    int fd;

    fd = open("./aio_read____not_notify.c", O_RDONLY);
    if (fd < 0)
        perror("./aio_read____not_notify.c");

    /* aio_read异步IO读(aio_write是类似的)
       本例：异步IO完成后，不通知进程 */
    
    memset(&aio, 0, sizeof(aio));
    aio.aio_fildes = fd;                /* 异步IO 的对象 */
    aio.aio_offset = 0;                 /* 文件偏移量(读/写操作从这开始)，fd已指定O_APPEN时，偏移量被忽略 */
#define MALLOC_LEN     1024
    aio.aio_buf = malloc(MALLOC_LEN);   /* 缓存(读到这里/从这里写到文件) */
    aio.aio_nbytes = 500;               /* 异步IO 想要读/写的字节数，不要超缓存 */
    aio.aio_lio_opcode = LIO_READ;      /* lio_listio 时才使用，用于指明读/写 */

    /* SIGEV_NONE：    异步IO完成后，不通知进程
       SIGEV_SIGNAL： 异步IO完成后，使用sigev_signo指定的信号通进程(SA_SIGINFO)，sigev_value信号参数
       SIGEV_THREAD： 异步IO完成后，在单独的分离线程中执行回调函数，，sigev_value回调参数 */
    aio.aio_sigevent.sigev_notify = SIGEV_NONE;


    if (aio_read(&aio) < 0)             /* 执行异步IO 读，完成前需保持aiocb稳定 */
        perror("aio_read");


    /* 以下两种等待方法都可以 */
#if 0
    /* 等待异步IO完成 */
    /* aio_error返回值
       0： 完成
       -1：aio_error调用失败，errno指明
       EINPROGRESS：异步IO 正在执行中，还未完成*/
    while (aio_error(&aio) == EINPROGRESS);
    ((char*)aio.aio_buf)[aio.aio_nbytes] = '\0';
#else
    /* 等待异步IO完成 */
    struct aiocb* aio_lst[4];
    aio_lst[0] = &aio;
    aio_suspend((void*)aio_lst, 1, NULL);  /* 挂起等待list中的前nent个 异步IO完成 */
    ((char*)aio.aio_buf)[aio.aio_nbytes] = '\0';
#endif


    printf("aio_return: %d\n\n", (int)aio_return(&aio)); /* 获取异步IO 返回值(与read/write/fsync返回值类似) */
    printf("%s\r\n", (char*)aio.aio_buf);
    free((void*)aio.aio_buf);


    close(fd);

    return 0;
}



