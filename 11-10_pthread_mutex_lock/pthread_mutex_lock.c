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
    pthread_mutex_t     mutex;
}MY_DATA;
MY_DATA shared_data;

void* thread_1(void* arg)
{
    printf("thread_1 run...\r\n");
    static int ret = 0;

    while (1)
    {
        pthread_mutex_lock(&shared_data.mutex);     /* �����ȴ������� */
        ret = ++shared_data.value;
        pthread_mutex_unlock(&shared_data.mutex);

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
        pthread_mutex_lock(&shared_data.mutex);     /* �����ȴ������� */
        ret = ++shared_data.value;
        pthread_mutex_unlock(&shared_data.mutex);

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

    /* ������mutex����������������Դ��ȷ��ͬһʱ��ֻ��һ���̷߳��ʹ�������
       pthread_mutex_init       ��ʼ��������
       pthread_mutex_lock       ��������������
       pthread_mutex_timedlock  �����������������г�ʱʱ��
       pthread_mutex_trylock    ����������������
       pthread_mutex_unlock     ����������
       pthread_mutex_destroy    ���ջ����� */

    pthread_mutex_init(&shared_data.mutex, 0);      /* ��ʼ�������� */
    shared_data.value = 0;

    pthread_create(&tid1, 0, thread_1, (void*)0);
    pthread_create(&tid2, 0, thread_2, (void*)0);
    pthread_join(tid1, (void*)&ret);    printf("thread_1 return [%d]\r\n", *ret);
    pthread_join(tid2, (void*)&ret);    printf("thread_2 return [%d]\r\n", *ret);

    pthread_mutex_destroy(&shared_data.mutex);      /* ���ջ����� */
           
    exit(0);
}



