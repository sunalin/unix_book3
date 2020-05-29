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

    /* aio_read�첽IO��(aio_write�����Ƶ�)
       �������첽IO��ɺ󣬲�֪ͨ���� */
    
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
    aio.aio_sigevent.sigev_notify = SIGEV_NONE;


    if (aio_read(&aio) < 0)             /* ִ���첽IO �������ǰ�豣��aiocb�ȶ� */
        perror("aio_read");


    /* �������ֵȴ����������� */
#if 0
    /* �ȴ��첽IO��� */
    /* aio_error����ֵ
       0�� ���
       -1��aio_error����ʧ�ܣ�errnoָ��
       EINPROGRESS���첽IO ����ִ���У���δ���*/
    while (aio_error(&aio) == EINPROGRESS);
    ((char*)aio.aio_buf)[aio.aio_nbytes] = '\0';
#else
    /* �ȴ��첽IO��� */
    struct aiocb* aio_lst[4];
    aio_lst[0] = &aio;
    aio_suspend((void*)aio_lst, 1, NULL);  /* ����ȴ�list�е�ǰnent�� �첽IO��� */
    ((char*)aio.aio_buf)[aio.aio_nbytes] = '\0';
#endif


    printf("aio_return: %d\n\n", (int)aio_return(&aio)); /* ��ȡ�첽IO ����ֵ(��read/write/fsync����ֵ����) */
    printf("%s\r\n", (char*)aio.aio_buf);
    free((void*)aio.aio_buf);


    close(fd);

    return 0;
}



