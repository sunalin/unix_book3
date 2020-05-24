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
    int                 cond_value; // ����ֵ
    pthread_mutex_t     mutex;
    pthread_cond_t      cond;
}MY_DATA;
MY_DATA my_data;

void* thread_1(void* arg)
{
    printf("thread_1 run...\r\n");
    static int ret = 0;

    while (1)
    {
        pthread_mutex_lock(&my_data.mutex);
        while (my_data.cond_value == 0)     // �ж���������ֵ������������ʱ����pthread_cond_wait
        {
            /* pthread_cond_wait����֮ǰ����pthread_mutex_lock��
               �����߰���ס��mutex����pthread_cond_wait��mutex����cond

               pthread_cond_wait
               {
                   1���̼߳���ȴ�����������(�ȴ�cond��������)
                   2��mutex����
                   3���̹߳���
                   4��(����cond��������/�ź�)�̻߳���
                   5��mutex����
                   6��pthread_cond_wait����
               }
               pthread_cond_timedwait   ����ʱʱ��

               pthread_cond_signal      ����֪ͨһ���ȴ�cond�������߳�(����cond��������)���ỽ���߳�
               pthread_cond_broadcast   ֪ͨȫ���ȴ�cond�������߳�(����cond��������)���ỽ���߳�

               ע�������ź�Ҳ���Ի����̣߳�ʹpthread_cond_wait���أ�����ͨ�����һ���������������֣�
                   pthread_cond_signal����֮ǰ�޸ı���ֵ��pthread_cond_wait���غ���������ֵ */
            pthread_cond_wait(&my_data.cond, &my_data.mutex);
            printf("pthread_cond_wait return...\r\n");
        }
        ret = my_data.cond_value;
        pthread_mutex_unlock(&my_data.mutex);

        printf("thread_1 value = [%d]\r\n", ret);
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
        pthread_mutex_lock(&my_data.mutex);
        my_data.cond_value = (~my_data.cond_value)&1;   // ������������ֵ
        ret = my_data.cond_value;
        pthread_mutex_unlock(&my_data.mutex);
        pthread_cond_signal(&my_data.cond);

        printf("thread_2 value = [%d]\r\n", ret);
        sleep(5);
    }
    
    pthread_exit((void*)&ret);
}

int main(int argc, char* args[])
{
    pthread_t tid1;
    pthread_t tid2;
    int* ret;
    

    pthread_mutex_init(&my_data.mutex, 0);          /* ��ʼ�������� */
    pthread_cond_init(&my_data.cond, 0);            /* ��ʼ���������� */
    my_data.cond_value = 0;

    pthread_create(&tid1, 0, thread_1, (void*)0);
    pthread_create(&tid2, 0, thread_2, (void*)0);
    pthread_join(tid1, (void*)&ret);    printf("thread_1 return [%d]\r\n", *ret);
    pthread_join(tid2, (void*)&ret);    printf("thread_2 return [%d]\r\n", *ret);

    pthread_mutex_destroy(&my_data.mutex);          /* ���ջ����� */
    pthread_cond_destroy(&my_data.cond);            /* ������������ */
           
    exit(0);
}



