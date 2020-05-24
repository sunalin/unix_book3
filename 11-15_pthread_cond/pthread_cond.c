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
    int                 cond_value; // 条件值
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
        while (my_data.cond_value == 0)     // 判断条件变量值，不符合条件时重新pthread_cond_wait
        {
            /* pthread_cond_wait调用之前必须pthread_mutex_lock，
               调用者把锁住的mutex传给pthread_cond_wait，mutex保护cond

               pthread_cond_wait
               {
                   1、线程加入等待条件队列中(等待cond条件发生)
                   2、mutex解锁
                   3、线程挂起
                   4、(满足cond条件发生/信号)线程唤醒
                   5、mutex加锁
                   6、pthread_cond_wait返回
               }
               pthread_cond_timedwait   带超时时间

               pthread_cond_signal      至少通知一个等待cond条件的线程(触发cond条件发生)，会唤醒线程
               pthread_cond_broadcast   通知全部等待cond条件的线程(触发cond条件发生)，会唤醒线程

               注：由于信号也可以唤醒线程，使pthread_cond_wait返回，所以通常会加一个条件变量来区分，
                   pthread_cond_signal调用之前修改变量值，pthread_cond_wait返回后判数变量值 */
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
        my_data.cond_value = (~my_data.cond_value)&1;   // 更改条件变量值
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
    

    pthread_mutex_init(&my_data.mutex, 0);          /* 初始化互斥量 */
    pthread_cond_init(&my_data.cond, 0);            /* 初始化条件变量 */
    my_data.cond_value = 0;

    pthread_create(&tid1, 0, thread_1, (void*)0);
    pthread_create(&tid2, 0, thread_2, (void*)0);
    pthread_join(tid1, (void*)&ret);    printf("thread_1 return [%d]\r\n", *ret);
    pthread_join(tid2, (void*)&ret);    printf("thread_2 return [%d]\r\n", *ret);

    pthread_mutex_destroy(&my_data.mutex);          /* 回收互斥量 */
    pthread_cond_destroy(&my_data.cond);            /* 回收条件变量 */
           
    exit(0);
}



