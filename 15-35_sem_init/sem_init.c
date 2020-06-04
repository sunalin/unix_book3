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

#include <stropts.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <limits.h>
#include <semaphore.h>  // -lpthread
#include <pthread.h>    // -lpthread


typedef struct
{
    char        name[_POSIX_NAME_MAX];
    sem_t*      sem;
}sem_name_t;

int sem_lock(sem_t* sem)
{
    /* 等待满足信号量值能够-1: 
       1、如果当前信号量值>0，信号量值-1后立即返回
       2、如果当前信号量值=0，阻塞等待信
       注: 信号量值总是>=0 */
    return sem_wait(sem);
}

int sem_trylock(sem_t* sem)
{
    /* 等待满足信号量值能够-1: 
       1、如果当前信号量值>0，信号量值-1后立即返回
       2、如果当前信号量值=0，阻塞等待信
       注: 信号量值总是>=0 */
    return sem_trywait(sem);    /* 不等待 */
}

int sem_timelock(sem_t* sem, const struct timespec* abstime)
{
    /* 等待满足信号量值能够-1: 
       1、如果当前信号量值>0，信号量值-1后立即返回
       2、如果当前信号量值=0，阻塞等待信
       注: 信号量值总是>=0 */
    return sem_timedwait(sem, abstime); /* 带超时等待 */
}

int sem_unlock(sem_t* sem)
{
    /* 使当前信号量值+1 */
    return sem_post(sem);
}




typedef struct
{
    int         value;
    sem_t       sem;
}MY_DATA;
MY_DATA shared_data = {.value = 0,};


void* thread_1(void* arg)
{
    pthread_detach(pthread_self());
    printf("thread_1 run...\r\n");

    int val;
    while (1)
    {
        sem_lock(&shared_data.sem);    /* 用信号量保护资源 */
        val = shared_data.value;
        sem_unlock(&shared_data.sem);

        printf("threadid[%lx] B: val[%d]\r\n", pthread_self(), val);
        if (val >= 4)
            break;
        sleep(1);
    }
    
    return 0;
}

int main(int argc, char* args[])
{
    /* POSIX未命名信号量，用于线程的的同步/互斥 */


    /* POSIX未命名信号量(也被称为内存信号量)
       pshared: 通常为0
       value:   0~SEM_VALUE_MAX 信号量初始值*/
    if (sem_init(&shared_data.sem, 0, 1) < 0)   /* 初始化未命名信号量 */
        perror("sem_init");


    pthread_t tid1;
    pthread_create(&tid1, 0, thread_1, (void*)0);

    int val;
    while (1)
    {    
        sem_lock(&shared_data.sem);    /* 用信号量保护资源 */
        val = ++shared_data.value;
        sem_unlock(&shared_data.sem);
    
        printf("threadid[%lx] A: val[%d]\r\n", pthread_self(), val);
        if (val >= 7)
            break;
        sleep(1);
    }

    if (sem_destroy(&shared_data.sem) < 0)      /* 释放未命名信号量 */
        perror("sem_destroy");

    /* 信号量同步/互斥
       https://blog.csdn.net/tennysonsky/article/details/46496201 */
    
    return 0;
}



