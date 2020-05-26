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

    /* 线程中fork：子进程只有一个线程，这个线程是fork调用者所在线程的副本
    
       pthread_atfork(prepar,parent,child)  注册fork处理函数，线程fork()时
       会调用这些函数，可注册多套处理函数，函数指针可以为0，多套parent与
       child函数的调用顺序与注册顺序一致，prepar调用顺序与注册顺序相反，
       一般用于清理锁或其它
            prepar：fork()创建子进程[前]，在父进程地址空间调用
            parent：fork()创建子进程[后]，在父进程地址空间调用
            child ：fork()创建子进程[后]，在子进程地址空间调用
            例如：在prepar中加锁，在parent与child中解锁，用于处理锁状态问题 */

    pthread_atfork(prepare_func, parent_func, child_func);    /* 注册fork处理函数 */
    pthread_create(&tid1, 0, thread_1, (void*)0);
    sleep(1);

    printf("pid[%d] fork() before...\r\n", getpid());
    pid = fork();
    printf("pid[%d] fork() return...\r\n", getpid());

    sleep(1);
    
    return 0;
}



