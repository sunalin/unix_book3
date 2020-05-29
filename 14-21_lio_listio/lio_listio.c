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


void aio_slot(sigval_t sigval)
{
    /* 在单独的分离线程中执行回调函数 */
    struct aiocb* aio = sigval.sival_ptr;
    
    printf("pid[%d] threadid[0x%lx] aio_slot: aio[%p]\r\n",
           getpid(),
           pthread_self(),
           aio);
    if (aio && aio_error(aio) == 0)
    {
        printf("pid[%d] threadid[0x%lx] aio_return[%d]\r\n",
               getpid(),
               pthread_self(),
               (int)aio_return(aio)); /* 获取异步IO 返回值(与read/write/fsync返回值类似) */
    }
    fflush(0);
}

int main(int argc, char* args[])
{
    struct aiocb* aio_lst[4];
    struct sigevent sig_cb;
    struct aiocb aio_rd;
    struct aiocb aio_wr;
    int fd_read;
    int fd_write;

    fd_read = open("./lio_listio.c", O_RDONLY);
    if (fd_read < 0)
        perror("./lio_listio.c");
    fd_write = open("./temp.out", O_WRONLY|O_CREAT|O_TRUNC);
    if (fd_write < 0)
        perror("./temp.out");

    
#define MALLOC_LEN     1024
    memset(&aio_rd, 0, sizeof(aio_rd));
    aio_rd.aio_fildes = fd_read;           /* 异步IO 的对象 */
    aio_rd.aio_offset = 0;                 /* 文件偏移量(读/写操作从这开始)，fd已指定O_APPEN时，偏移量被忽略 */
    aio_rd.aio_buf = malloc(MALLOC_LEN);   /* 缓存(读到这里/从这里写到文件) */
    aio_rd.aio_nbytes = 100;               /* 异步IO 想要读/写的字节数，不要超缓存 */
    aio_rd.aio_lio_opcode = LIO_READ;      /* lio_listio 时才使用，用于指明读/写 */
    /* SIGEV_NONE：    异步IO完成后，不通知进程
       SIGEV_SIGNAL： 异步IO完成后，使用sigev_signo指定的信号通进程(SA_SIGINFO)，sigev_value信号参数
       SIGEV_THREAD： 异步IO完成后，在单独的分离线程中执行回调函数，，sigev_value回调参数 */
    aio_rd.aio_sigevent.sigev_notify = SIGEV_THREAD;
    aio_rd.aio_sigevent.sigev_notify_function = aio_slot;    /* 指定线程回调，回调函数是在另一个线程执行的 */
    aio_rd.aio_sigevent.sigev_notify_attributes = NULL;
    aio_rd.aio_sigevent.sigev_value.sival_ptr = &aio_rd;


    memset(&aio_wr, 0, sizeof(aio_wr));
    aio_wr.aio_fildes = fd_write;          /* 异步IO 的对象 */
    aio_wr.aio_offset = 0;                 /* 文件偏移量(读/写操作从这开始)，fd已指定O_APPEN时，偏移量被忽略 */
    aio_wr.aio_buf = malloc(MALLOC_LEN);   /* 缓存(读到这里/从这里写到文件) */
    memset((void*)aio_wr.aio_buf, 'a', MALLOC_LEN);
    aio_wr.aio_nbytes = 150;               /* 异步IO 想要读/写的字节数，不要超缓存 */
    aio_wr.aio_lio_opcode = LIO_WRITE;     /* lio_listio 时才使用，用于指明读/写 */
    /* SIGEV_NONE：    异步IO完成后，不通知进程
       SIGEV_SIGNAL： 异步IO完成后，使用sigev_signo指定的信号通进程(SA_SIGINFO)，sigev_value信号参数
       SIGEV_THREAD： 异步IO完成后，在单独的分离线程中执行回调函数，，sigev_value回调参数 */
    aio_wr.aio_sigevent.sigev_notify = SIGEV_THREAD;
    aio_wr.aio_sigevent.sigev_notify_function = aio_slot;    /* 指定线程回调，回调函数是在另一个线程执行的 */
    aio_wr.aio_sigevent.sigev_notify_attributes = NULL;
    aio_wr.aio_sigevent.sigev_value.sival_ptr = &aio_wr;


    printf("pid[%d] threadid[0x%lx] main\r\n",
           getpid(),
           pthread_self());
    aio_lst[0] = &aio_rd;
    aio_lst[1] = &aio_wr;


    /* lio_listio(list,nent,sigev)    函数同时发起多个异步(一次内核上下文切换中启动大量的IO操作)

       mode=LIO_WAIT：   会阻塞这个调用，直到所有IO都完成，sigev被忽略
       mode=LIO_NOWAIT：在操作进行排队之后， 就会返回，可sigev用于指定通知，所有IO都完成时会通知(这个是单独的)
       aiocb.aio_lio_opcode=(LIO_READ/LIO_WRITELIO_NOP)用于指明读/写 */

    /* 以下两种模式都已测试 */
#if 0   /* mode=LIO_WAIT */
    lio_listio(LIO_WAIT, aio_lst, 2, NULL);         /* 阻塞等待IO完成 */
#else   /* mode=LIO_NOWAIT */
    sig_cb.sigev_notify = SIGEV_THREAD;
    sig_cb.sigev_notify_function = aio_slot;
    sig_cb.sigev_notify_attributes = NULL;
    sig_cb.sigev_value.sival_ptr = 0;   // 回调参数
    lio_listio(LIO_NOWAIT, aio_lst, 2, &sig_cb);    /* 非阻塞等待IO完成 */
    aio_suspend((void*)aio_lst, 2, NULL);           /* 等待IO完成 */
#endif
    
    sleep(1);
    //aio_fsync(O_SYNC, &aio_rd);
    //aio_fsync(O_SYNC, &aio_wr);
    fflush(0);
    close(fd_read);
    close(fd_write);

    /* 以下是打印部分 */
    printf("=================================================\r\n");
    ((char*)aio_rd.aio_buf)[aio_rd.aio_nbytes] = '\0';
    printf("%s\r\n", (char*)aio_rd.aio_buf);
    free((void*)aio_rd.aio_buf);
    free((void*)aio_wr.aio_buf);
    printf("=================================================\r\n");
    system("rm ./temp.out|cat ./temp.out");
    printf("\r\n");

    return 0;
}



