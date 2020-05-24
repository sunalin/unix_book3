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


pthread_barrier_t barrier;  /* 屏障barrier */

void* thread_1(void* arg)
{
    printf("thread_1 run...\r\n");
    static int ret = 0;

    pthread_barrier_wait(&barrier); /* 屏障数+1，未达到总屏障数之前线程休眠，达到后所有线程唤醒 继续往下执行 */
    printf("thread_1 barrier\r\n");
    
    pthread_exit((void*)&ret);
}

void* thread_2(void* arg)
{
    printf("thread_2 run...\r\n");
    static int ret = 0;

    pthread_barrier_wait(&barrier); /* 屏障数+1，未达到总屏障数之前线程休眠，达到后所有线程唤醒 继续往下执行 */
    printf("thread_2 barrier\r\n");
    
    pthread_exit((void*)&ret);
}

int main(int argc, char* args[])
{
    pthread_t tid1;
    pthread_t tid2;
    int* ret;

    /* 屏障barrier：协调多个线程并行工作的同步机制
                    (可理解成：要等所有线程都执行到pthread_barrier_wait之后，所有线程才能继续向下执行)

       pthread_barrier_init       初始化屏障 初始化总屏障数
       pthread_barrier_wait       屏障数+1，未达到总屏障数之前线程休眠，达到后所有线程唤醒 继续往下执行
       pthread_barrier_destroy    回收屏障 */

    pthread_barrier_init(&barrier, 0, 3);   /* 初始化屏障 总屏障数3，我们的例子有3个线程 */

    pthread_create(&tid1, 0, thread_1, (void*)0);
    pthread_create(&tid2, 0, thread_2, (void*)0);

    pthread_barrier_wait(&barrier); /* 屏障数+1，未达到总屏障数之前线程休眠，达到后所有线程唤醒 继续往下执行 */
    printf("main barrier\r\n");
    
    pthread_join(tid1, (void*)&ret);    printf("thread_1 return [%d]\r\n", *ret);
    pthread_join(tid2, (void*)&ret);    printf("thread_2 return [%d]\r\n", *ret);

    pthread_barrier_destroy(&barrier);      /* 回收屏障 */
           
    exit(0);
}



