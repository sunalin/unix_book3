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


#define NOTIFY_SIGNAL   (SIGRTMIN)
static int return_flag = 0;


void aio_slot(int signo, siginfo_t* info, void* context)
{
    printf("pid[%d] threadid[0x%lx] recv signal: %2d[%s]\r\n",
           getpid(),
           pthread_self(),
           info->si_signo, strsignal(info->si_signo));

    if (info->si_signo == NOTIFY_SIGNAL)
    {
        struct aiocb* aio = info->si_value.sival_ptr; 
        if (aio_error(aio) == 0)
        {
            printf("aio_return: %d\n\n",
                   (int)aio_return(aio)); /* ��ȡ�첽IO ����ֵ(��read/write/fsync����ֵ����) */
        }

        return_flag = 1;
    }

}

int main(int argc, char* args[])
{
    struct sigaction sig_act;
    struct aiocb aio;
    int fd;

    fd = open("./aio_read____signal_notify.c", O_RDONLY);
    if (fd < 0)
        perror("./aio_read____signal_notify.c");

    /* aio_read�첽IO��(aio_write�����Ƶ�)
       �������첽IO��ɺ�ʹ��ָ���ź�֪ͨ���� */
    
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
    aio.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    aio.aio_sigevent.sigev_signo = NOTIFY_SIGNAL;    /* ָ��֪ͨ�ź� */
    aio.aio_sigevent.sigev_value.sival_ptr = &aio;

    /* ��׽�ź� */
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags = SA_SIGINFO;
    sig_act.sa_sigaction = aio_slot;
    sigaction(NOTIFY_SIGNAL, &sig_act, NULL);


    if (aio_read(&aio) < 0)             /* ִ���첽IO �������ǰ�豣��aiocb�ȶ� */
        perror("aio_read");

    printf("pid[%d] threadid[0x%lx] main\r\n",
           getpid(),
           pthread_self());
    while (return_flag == 0)            /* �ȴ� �첽IO֪ͨ�źų��� */
        sleep(1);

    ((char*)aio.aio_buf)[aio.aio_nbytes] = '\0';
    printf("%s\r\n", (char*)aio.aio_buf);
    free((void*)aio.aio_buf);


    close(fd);

    return 0;
}



