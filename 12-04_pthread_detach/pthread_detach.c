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


void* thread_1(void* arg)
{
    pthread_detach(pthread_self());
    printf("thread_1 run...\r\n");

    static int ret_val = 0;
    
    pthread_exit(&ret_val);
}

void* thread_2(void* arg)
{
    pthread_detach(pthread_self());
    printf("thread_2 run...\r\n");

    static int ret_val = 0;
    
    pthread_exit(&ret_val);
}

int main(int argc, char* args[])
{
    pthread_t tid1;
    pthread_t tid2;
    pthread_attr_t attr2;

    /* �����̣߳��������̵߳ķ���ֵ״̬ʱ����ʹ�÷����߳�(ָ��detach����)��
                 �����߳���ֹ���Զ�������Դ���Է����̲߳���ʹ�� pthread_join 

       �����̷߳�����
           1������pthread_detach(pthread_t)�����������̵߳��ã�Ҳ�����߳����ѵ���
           2��pthread_create ��2������ attr ָ�� PTHREAD_CREATE_DETACHED ���� */

    printf("main run...\r\n");

    pthread_create(&tid1, 0, thread_1, (void*)0);
    pthread_detach(tid1);
    
    pthread_attr_init(&attr2);
    pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_DETACHED);
    pthread_create(&tid2, &attr2, thread_2, (void*)0);

    sleep(1);   // �����߳���ִ�еĻ���
    
    exit(0);
}



