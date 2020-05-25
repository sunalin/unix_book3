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
    pthread_key_t   key;
    pthread_once_t  once;
}MY_DATA;
MY_DATA private_data = {.once = PTHREAD_ONCE_INIT,};

void buf_destroy(void* obj)
{
    printf("thread id:%lx \t free(%p)\r\n", pthread_self(), obj);
    free(obj);
}

void buf_key_init(void)
{
    /* pthread_key_create ����һ���߳�˽������key����ָ��key����������������
       key�������̶߳�����ʹ�ø�key��ÿ�߳̿ɸ����Լ�����Ҫ��key������ͬ����
       �ݶ����൱���ṩ��һ��ͬ������ֵͬ��ȫ�ֱ���(һ����ֵ)��key�൱�ڱ�
       ������ÿ���̴߳���ʱ�������ݶ����ǿ�ֵ0��ͨ����malloc����

       pthread_setspecific(key)     ���õ�ǰ�߳�˽�����ݶ���
       pthread_getspecific(key)     ��ȡ��ǰ�߳�˽�����ݶ���
       pthread_key_delete(key)      ȡ��key���߳�˽�����ݵĹ��������ἤ��������������ǰ�ֶ�free
       
       ���߳�pthread_exit�򷵻�ʱ�����(key�������ݶ���ֵ�ǿ�ʱ)��(��key�������ݶ���Ϊ
       ����)��������������ֵΪ��ʱ�����������������exit�������쳣��ֹʱ���������������
       ע����key�������ݶ���Ϊmalloc������ڴ�ʱ�����Ժ���ʹ����������free */
    pthread_key_create(&private_data.key, buf_destroy);    
    printf("pthread_key_create\r\n");
}

void buf_init(void)
{
    /* �ڶ���̵߳���pthread_onceʱ(���ͬһ��pthread_once_t)��
       pthread_onceָ���ĺ���ֻ�ᱻ����һ�� */
    pthread_once(&private_data.once, buf_key_init);      /* ֻ�����һ��buf_key_init */

    char* tmp = pthread_getspecific(private_data.key);   /* ��ȡkey�������ݶ��� */
    if (tmp == 0)   /* ÿ���̴߳���ʱ���������ǿ�ֵ0 */
    {
        tmp = malloc(256); /* ָ��malloc�ڴ棬Ҳ����ָ��������ַ */
        if (tmp)
            pthread_setspecific(private_data.key, tmp);  /* ����key�������ݶ��� */
        printf("thread id:%lx \t malloc = %p\r\n", pthread_self(), tmp);
    }
}

void* buf_get(void)
{
    return pthread_getspecific(private_data.key);
}

void* thread_1(void* arg)
{
    pthread_detach(pthread_self());
    printf("thread_1 run...\r\n");
    static int ret = 0;

    buf_init();
    int* dat = buf_get();
    *dat = 100;
    while (1)
    {
        dat = buf_get();
        (*dat)++;

        printf("thread_1 dat = [%d]\r\n", *dat);
        if (*dat >= 110)
            break;
        sleep(1);
    }
    
    pthread_exit((void*)&ret);
}

void* thread_2(void* arg)
{
    pthread_detach(pthread_self());
    printf("thread_2 run...\r\n");
    static int ret = 0;

    buf_init();
    int* dat = buf_get();
    *dat = 10000;
    while (1)
    {
        dat = buf_get();
        (*dat)++;

        printf("thread_2 dat = [%d]\r\n", *dat);
        if (*dat >= 10010)
            break;
        sleep(1);
    }
    
    pthread_exit((void*)&ret);
}

int main(int argc, char* args[])
{
    pthread_t tid1;
    pthread_t tid2;

    pthread_create(&tid1, 0, thread_1, (void*)0);
    pthread_create(&tid2, 0, thread_2, (void*)0);

    buf_init();
    int* dat = buf_get();
    *dat = 0;
    while (1)
    {
        dat = buf_get();
        (*dat)++;

        printf("main dat = [%d]\r\n", *dat);
        if (*dat >= 13)
            break;
        sleep(1);
    }
    
    return 0;
}



