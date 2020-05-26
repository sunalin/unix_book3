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


pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void prepare_func(void)
{
    printf("pid[%d] prepare_func \t[lock]\r\n", getpid());
    pthread_mutex_lock(&mutex1);
    pthread_mutex_lock(&mutex2);
}

void parent_func(void)
{
    printf("pid[%d] parent_func \t[unlock]\r\n", getpid());
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
}

void child_func(void)
{
    printf("pid[%d] child_func \t[unlock]\r\n", getpid());
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
}

void* thread_1(void* arg)
{
    pthread_detach(pthread_self());
    printf("pid[%d] thread_1 run...\r\n", getpid());
    static int ret = 0;

    while (1)
        sleep(1);
    
    pthread_exit((void*)&ret);
}

int main(int argc, char* args[])
{
    pid_t pid;
    pthread_t tid1;

    /* �߳���fork���ӽ���ֻ��һ���̣߳�����߳���fork�����������̵߳ĸ���
    
       pthread_atfork(prepar,parent,child)  ע��fork���������߳�fork()ʱ
       �������Щ��������ע����״�����������ָ�����Ϊ0������parent��
       child�����ĵ���˳����ע��˳��һ�£�prepar����˳����ע��˳���෴��
       һ������������������
            prepar��fork()�����ӽ���[ǰ]���ڸ����̵�ַ�ռ����
            parent��fork()�����ӽ���[��]���ڸ����̵�ַ�ռ����
            child ��fork()�����ӽ���[��]�����ӽ��̵�ַ�ռ����
            ���磺��prepar�м�������parent��child�н��������ڴ�����״̬���� */

    pthread_atfork(prepare_func, parent_func, child_func);    /* ע��fork������ */
    pthread_create(&tid1, 0, thread_1, (void*)0);
    sleep(1);

    printf("pid[%d] fork() before...\r\n", getpid());
    pid = fork();
    printf("pid[%d] fork() return...\r\n", getpid());

    sleep(1);
    
    return 0;
}



