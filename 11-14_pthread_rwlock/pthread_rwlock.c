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
    pthread_rwlock_t    rwlock;
}MY_DATA;
MY_DATA shared_data;

void* thread_1(void* arg)
{
    printf("thread_1 run...\r\n");
    static int ret = 0;

    while (1)
    {
        pthread_rwlock_wrlock(&shared_data.rwlock);     /* �����ȴ� д�� */
        ret = ++shared_data.value;
        pthread_rwlock_unlock(&shared_data.rwlock);

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
        pthread_rwlock_rdlock(&shared_data.rwlock);     /* �����ȴ� ���� */
        ret = shared_data.value;
        pthread_rwlock_unlock(&shared_data.rwlock);

        printf("thread_2 value = [%d]\r\n", ret);
        if (shared_data.value >= 10)
            break;
        sleep(2);
    }
    
    pthread_exit((void*)&ret);
}

int main(int argc, char* args[])
{
    pthread_t tid1;
    pthread_t tid2;
    int* ret;

    /* ��д��rwlock�����ʺ� ������>д���� �ı�������������Դ
       1������д��״̬ʱ���ڽ���֮ǰ��ͼ��ȡ�����̶߳��ᱻ������
          һ��ֻ��һ���߳̿���ռ��д��
       2�����ڶ���״̬ʱ����ͼ��ȡ�������̶߳����Եõ�����Ȩ��
          ��ʱд�����󽫱�������ֱ�������߳��ͷŶ���������д��
          ����֮�������µĶ������󽫱�����(��ֹ�������ڱ�ռ��)
       pthread_rwlock_init          ��ʼ����д��
       pthread_rwlock_rdlock        �����ȴ� ����
       pthread_rwlock_wrlock        �����ȴ� д��
       pthread_rwlock_timedrdlock   �����ȴ� �������г�ʱʱ��
       pthread_rwlock_timedwrlock   �����ȴ� д�����г�ʱʱ��
       pthread_rwlock_tryrdlock     �������ȴ� ����
       pthread_rwlock_trywrlock     �������ȴ� д��
       pthread_rwlock_unlock        ������д��
       pthread_rwlock_destroy       ���ն�д�� */


    pthread_rwlock_init(&shared_data.rwlock, 0);      /* ��ʼ����д�� */
    shared_data.value = 0;

    pthread_create(&tid1, 0, thread_1, (void*)0);
    pthread_create(&tid2, 0, thread_2, (void*)0);
    pthread_join(tid1, (void*)&ret);    printf("thread_1 return [%d]\r\n", *ret);
    pthread_join(tid2, (void*)&ret);    printf("thread_2 return [%d]\r\n", *ret);

    pthread_rwlock_destroy(&shared_data.rwlock);      /* ���ջ����� */
           
    exit(0);
}



