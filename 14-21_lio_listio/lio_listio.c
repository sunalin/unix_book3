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
    /* �ڵ����ķ����߳���ִ�лص����� */
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
               (int)aio_return(aio)); /* ��ȡ�첽IO ����ֵ(��read/write/fsync����ֵ����) */
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
    aio_rd.aio_fildes = fd_read;           /* �첽IO �Ķ��� */
    aio_rd.aio_offset = 0;                 /* �ļ�ƫ����(��/д�������⿪ʼ)��fd��ָ��O_APPENʱ��ƫ���������� */
    aio_rd.aio_buf = malloc(MALLOC_LEN);   /* ����(��������/������д���ļ�) */
    aio_rd.aio_nbytes = 100;               /* �첽IO ��Ҫ��/д���ֽ�������Ҫ������ */
    aio_rd.aio_lio_opcode = LIO_READ;      /* lio_listio ʱ��ʹ�ã�����ָ����/д */
    /* SIGEV_NONE��    �첽IO��ɺ󣬲�֪ͨ����
       SIGEV_SIGNAL�� �첽IO��ɺ�ʹ��sigev_signoָ�����ź�ͨ����(SA_SIGINFO)��sigev_value�źŲ���
       SIGEV_THREAD�� �첽IO��ɺ��ڵ����ķ����߳���ִ�лص���������sigev_value�ص����� */
    aio_rd.aio_sigevent.sigev_notify = SIGEV_THREAD;
    aio_rd.aio_sigevent.sigev_notify_function = aio_slot;    /* ָ���̻߳ص����ص�����������һ���߳�ִ�е� */
    aio_rd.aio_sigevent.sigev_notify_attributes = NULL;
    aio_rd.aio_sigevent.sigev_value.sival_ptr = &aio_rd;


    memset(&aio_wr, 0, sizeof(aio_wr));
    aio_wr.aio_fildes = fd_write;          /* �첽IO �Ķ��� */
    aio_wr.aio_offset = 0;                 /* �ļ�ƫ����(��/д�������⿪ʼ)��fd��ָ��O_APPENʱ��ƫ���������� */
    aio_wr.aio_buf = malloc(MALLOC_LEN);   /* ����(��������/������д���ļ�) */
    memset((void*)aio_wr.aio_buf, 'a', MALLOC_LEN);
    aio_wr.aio_nbytes = 150;               /* �첽IO ��Ҫ��/д���ֽ�������Ҫ������ */
    aio_wr.aio_lio_opcode = LIO_WRITE;     /* lio_listio ʱ��ʹ�ã�����ָ����/д */
    /* SIGEV_NONE��    �첽IO��ɺ󣬲�֪ͨ����
       SIGEV_SIGNAL�� �첽IO��ɺ�ʹ��sigev_signoָ�����ź�ͨ����(SA_SIGINFO)��sigev_value�źŲ���
       SIGEV_THREAD�� �첽IO��ɺ��ڵ����ķ����߳���ִ�лص���������sigev_value�ص����� */
    aio_wr.aio_sigevent.sigev_notify = SIGEV_THREAD;
    aio_wr.aio_sigevent.sigev_notify_function = aio_slot;    /* ָ���̻߳ص����ص�����������һ���߳�ִ�е� */
    aio_wr.aio_sigevent.sigev_notify_attributes = NULL;
    aio_wr.aio_sigevent.sigev_value.sival_ptr = &aio_wr;


    printf("pid[%d] threadid[0x%lx] main\r\n",
           getpid(),
           pthread_self());
    aio_lst[0] = &aio_rd;
    aio_lst[1] = &aio_wr;


    /* lio_listio(list,nent,sigev)    ����ͬʱ�������첽(һ���ں��������л�������������IO����)

       mode=LIO_WAIT��   ������������ã�ֱ������IO����ɣ�sigev������
       mode=LIO_NOWAIT���ڲ��������Ŷ�֮�� �ͻ᷵�أ���sigev����ָ��֪ͨ������IO�����ʱ��֪ͨ(����ǵ�����)
       aiocb.aio_lio_opcode=(LIO_READ/LIO_WRITELIO_NOP)����ָ����/д */

    /* ��������ģʽ���Ѳ��� */
#if 0   /* mode=LIO_WAIT */
    lio_listio(LIO_WAIT, aio_lst, 2, NULL);         /* �����ȴ�IO��� */
#else   /* mode=LIO_NOWAIT */
    sig_cb.sigev_notify = SIGEV_THREAD;
    sig_cb.sigev_notify_function = aio_slot;
    sig_cb.sigev_notify_attributes = NULL;
    sig_cb.sigev_value.sival_ptr = 0;   // �ص�����
    lio_listio(LIO_NOWAIT, aio_lst, 2, &sig_cb);    /* �������ȴ�IO��� */
    aio_suspend((void*)aio_lst, 2, NULL);           /* �ȴ�IO��� */
#endif
    
    sleep(1);
    //aio_fsync(O_SYNC, &aio_rd);
    //aio_fsync(O_SYNC, &aio_wr);
    fflush(0);
    close(fd_read);
    close(fd_write);

    /* �����Ǵ�ӡ���� */
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



