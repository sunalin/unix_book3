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


pthread_barrier_t barrier;  /* ����barrier */

void* thread_1(void* arg)
{
    printf("thread_1 run...\r\n");
    static int ret = 0;

    pthread_barrier_wait(&barrier); /* ������+1��δ�ﵽ��������֮ǰ�߳����ߣ��ﵽ�������̻߳��� ��������ִ�� */
    printf("thread_1 barrier\r\n");
    
    pthread_exit((void*)&ret);
}

void* thread_2(void* arg)
{
    printf("thread_2 run...\r\n");
    static int ret = 0;

    pthread_barrier_wait(&barrier); /* ������+1��δ�ﵽ��������֮ǰ�߳����ߣ��ﵽ�������̻߳��� ��������ִ�� */
    printf("thread_2 barrier\r\n");
    
    pthread_exit((void*)&ret);
}

int main(int argc, char* args[])
{
    pthread_t tid1;
    pthread_t tid2;
    int* ret;

    /* ����barrier��Э������̲߳��й�����ͬ������
                    (�����ɣ�Ҫ�������̶߳�ִ�е�pthread_barrier_wait֮�������̲߳��ܼ�������ִ��)

       pthread_barrier_init       ��ʼ������ ��ʼ����������
       pthread_barrier_wait       ������+1��δ�ﵽ��������֮ǰ�߳����ߣ��ﵽ�������̻߳��� ��������ִ��
       pthread_barrier_destroy    �������� */

    pthread_barrier_init(&barrier, 0, 3);   /* ��ʼ������ ��������3�����ǵ�������3���߳� */

    pthread_create(&tid1, 0, thread_1, (void*)0);
    pthread_create(&tid2, 0, thread_2, (void*)0);

    pthread_barrier_wait(&barrier); /* ������+1��δ�ﵽ��������֮ǰ�߳����ߣ��ﵽ�������̻߳��� ��������ִ�� */
    printf("main barrier\r\n");
    
    pthread_join(tid1, (void*)&ret);    printf("thread_1 return [%d]\r\n", *ret);
    pthread_join(tid2, (void*)&ret);    printf("thread_2 return [%d]\r\n", *ret);

    pthread_barrier_destroy(&barrier);      /* �������� */
           
    exit(0);
}



