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
    /* �ڵ����ķ����߳���ִ�лص����� */
    struct aiocb* aio = sigval.sival_ptr;
    
    printf("pid[%d] threadid[0x%lx] aio_slot\r\n",
           getpid(),
           pthread_self());

    if (aio_error(aio) == 0)
    {
        printf("aio_return: %d\n\n",
               (int)aio_return(aio)); /* ��ȡ�첽IO ����ֵ(��read/write/fsync����ֵ����) */
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

    /* aio_read�첽IO��(aio_write�����Ƶ�)
       �������첽IO��ɺ��ڵ����ķ����߳���ִ�лص�������ע��ͬ������ */
    
    memset(&aio, 0, sizeof(aio));
    aio.aio_fildes = fd;                /* �첽IO �Ķ��� */
    aio.aio_offset = 0;                 /* �ļ�ƫ����(��/д�������⿪ʼ)��fd��ָ��O_APPENʱ��ƫ���������� */
#define MALLOC_LEN     1024
    aio.aio_buf = malloc(MALLOC_LEN);   /* ����(��������/������д���ļ�) */
    aio.aio_nbytes = 500;               /* �첽IO ��Ҫ��/д���ֽ�������Ҫ������ */
    aio.aio_lio_opcode = LIO_READ;      /* lio_listio ʱ��ʹ�ã�����ָ����/д */

    /* SIGEV_NONE��    �첽IO��ɺ󣬲�֪ͨ����
       SIGEV_SIGNAL�� �첽IO��ɺ�ʹ��sigev_signoָ�����ź�ͨ����(SA_SIGINFO)��sigev_value�źŲ���
       SIGEV_THREAD�� �첽IO��ɺ��ڵ����ķ����߳���ִ�лص���������sigev_value�ص����� */
    aio.aio_sigevent.sigev_notify = SIGEV_THREAD;
    aio.aio_sigevent.sigev_notify_function = aio_slot;    /* ָ���̻߳ص����ص�����������һ���߳�ִ�е� */
    aio.aio_sigevent.sigev_notify_attributes = NULL;
    aio.aio_sigevent.sigev_value.sival_ptr = &aio;


    if (aio_read(&aio) < 0)             /* ִ���첽IO �������ǰ�豣��aiocb�ȶ� */
        perror("aio_read");

    printf("pid[%d] threadid[0x%lx] main\r\n",
           getpid(),
           pthread_self());
    while (1)
    {
        pthread_mutex_lock(&return_flag.mutex);
        if (return_flag.value == 1)     /* �ȴ� �첽IO�̻߳ص� */
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



