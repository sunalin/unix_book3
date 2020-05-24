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
        pthread_rwlock_wrlock(&shared_data.rwlock);     /* 阻塞等待 写锁 */
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
        pthread_rwlock_rdlock(&shared_data.rwlock);     /* 阻塞等待 读锁 */
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

    /* 读写锁rwlock：更适合 读次数>写次数 的保护共享数据资源
       1、处于写锁状态时，在解锁之前试图获取锁的线程都会被阻塞，
          一次只有一个线程可以占有写锁
       2、处于读锁状态时，试图获取读锁的线程都可以得到访问权，
          这时写锁请求将被阻塞，直到所有线程释放读锁，不过写锁
          请求之后所有新的读锁请求将被阻塞(防止读锁长期被占用)
       pthread_rwlock_init          初始化读写锁
       pthread_rwlock_rdlock        阻塞等待 读锁
       pthread_rwlock_wrlock        阻塞等待 写锁
       pthread_rwlock_timedrdlock   阻塞等待 读锁，有超时时间
       pthread_rwlock_timedwrlock   阻塞等待 写锁，有超时时间
       pthread_rwlock_tryrdlock     非阻塞等待 读锁
       pthread_rwlock_trywrlock     非阻塞等待 写锁
       pthread_rwlock_unlock        解锁读写锁
       pthread_rwlock_destroy       回收读写锁 */


    pthread_rwlock_init(&shared_data.rwlock, 0);      /* 初始化读写锁 */
    shared_data.value = 0;

    pthread_create(&tid1, 0, thread_1, (void*)0);
    pthread_create(&tid2, 0, thread_2, (void*)0);
    pthread_join(tid1, (void*)&ret);    printf("thread_1 return [%d]\r\n", *ret);
    pthread_join(tid2, (void*)&ret);    printf("thread_2 return [%d]\r\n", *ret);

    pthread_rwlock_destroy(&shared_data.rwlock);      /* 回收互斥量 */
           
    exit(0);
}



