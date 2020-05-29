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


typedef struct
{
    int                 value;
    pthread_mutex_t     mutex;
}MY_DATA;
MY_DATA return_flag =
{
    .value = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
};


void aio_slot(sigval_t sigval)
{
    /* 在单独的分离线程中执行回调函数 */
    struct aiocb* aio = sigval.sival_ptr;
    
    printf("pid[%d] threadid[0x%lx] aio_slot\r\n",
           getpid(),
           pthread_self());

    if (aio_error(aio) == 0)
    {
        printf("aio_return: %d\n\n",
               (int)aio_return(aio)); /* 获取异步IO 返回值(与read/write/fsync返回值类似) */
    }

    pthread_mutex_lock(&return_flag.mutex);
    return_flag.value = 1;
    pthread_mutex_unlock(&return_flag.mutex);
}

int main(int argc, char* args[])
{
    struct sigaction sig_act;
    struct aiocb aio;
    int fd;

    fd = open("./aio_read____thread_callback_notify.c", O_RDONLY);
    if (fd < 0)
        perror("./aio_read____thread_callback_notify.c");

    /* aio_read异步IO读(aio_write是类似的)
       本例：异步IO完成后，在单独的分离线程中执行回调函数，注意同步问题 */
    
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
    aio.aio_sigevent.sigev_notify = SIGEV_THREAD;
    aio.aio_sigevent.sigev_notify_function = aio_slot;    /* 指定线程回调，回调函数是在另一个线程执行的 */
    aio.aio_sigevent.sigev_notify_attributes = NULL;
    aio.aio_sigevent.sigev_value.sival_ptr = &aio;


    if (aio_read(&aio) < 0)             /* 执行异步IO 读，完成前需保持aiocb稳定 */
        perror("aio_read");

    printf("pid[%d] threadid[0x%lx] main\r\n",
           getpid(),
           pthread_self());
    while (1)
    {
        pthread_mutex_lock(&return_flag.mutex);
        if (return_flag.value == 1)     /* 等待 异步IO线程回调 */
        {
            pthread_mutex_unlock(&return_flag.mutex);
            break;
        }        
        pthread_mutex_unlock(&return_flag.mutex);
        sleep(1);
    }

    ((char*)aio.aio_buf)[aio.aio_nbytes] = '\0';
    printf("%s\r\n", (char*)aio.aio_buf);
    free((void*)aio.aio_buf);


    close(fd);

    return 0;
}



