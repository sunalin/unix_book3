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
#include <pthread.h>


typedef struct
{
    int                 value;
    pthread_spinlock_t  spinlock;
}MY_DATA;
MY_DATA shared_data;

void* thread_1(void* arg)
{
    printf("thread_1 run...\r\n");
    static int ret = 0;

    while (1)
    {
        pthread_spin_lock(&shared_data.spinlock);     /* �����ȴ������� */
        ret = ++shared_data.value;
        pthread_spin_unlock(&shared_data.spinlock);

        printf("thread_1 value = [%d]\r\n", ret);
        if (shared_data.value >= 10)
            break;
        sleep(1);
    }
    
    pthread_exit((void*)&ret);
}

void* thread_2(void* arg)
{
    printf("thread_2 run...\r\n");
    static int ret = 0;

    while (1)
    {
        pthread_spin_lock(&shared_data.spinlock);     /* �����ȴ������� */
        ret = ++shared_data.value;
        pthread_spin_unlock(&shared_data.spinlock);

        printf("thread_2 value = [%d]\r\n", ret);
        if (shared_data.value >= 12)
            break;
        sleep(1);
    }
    
    pthread_exit((void*)&ret);
}

int main(int argc, char* args[])
{
    pthread_t tid1;
    pthread_t tid2;
    int* ret;

    /* ������spin����mutex���ƣ�������������ͨ������ʹ�߳������������ڻ�ȡ��֮ǰһ
                   ֱ����æ��(����)����״̬�������ù�ռ��CPU������������������������������е�ʱ��̣�
                   ���̲߳���ϣ�������µ����Ϸ�̫��ɱ���
       pthread_spin_init       ��ʼ�������� PTHREAD_PROCESS_PRIVATE PTHREAD_PROCESS_SHARED
       pthread_spin_lock       ��������������(�̲߳�˯�ߣ����������˷�CPU��Դ)��return 0��ʱ�Ѽ���
       pthread_spin_trylock    ������������������return 0��ʱ�Ѽ���
       pthread_spin_unlock     ����������
       pthread_spin_destroy    ���������� */

    pthread_spin_init(&shared_data.spinlock, PTHREAD_PROCESS_PRIVATE);      /* ��ʼ�������� */
    shared_data.value = 0;

    pthread_create(&tid1, 0, thread_1, (void*)0);
    pthread_create(&tid2, 0, thread_2, (void*)0);
    pthread_join(tid1, (void*)&ret);    printf("thread_1 return [%d]\r\n", *ret);
    pthread_join(tid2, (void*)&ret);    printf("thread_2 return [%d]\r\n", *ret);

    pthread_spin_destroy(&shared_data.spinlock);      /* ���������� */
           
    exit(0);
}



