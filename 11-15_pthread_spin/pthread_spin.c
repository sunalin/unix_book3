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
    pthread_spinlock_t  spinlock;
}MY_DATA;
MY_DATA shared_data;

void* thread_1(void* arg)
{
    printf("thread_1 run...\r\n");
    static int ret = 0;

    while (1)
    {
        pthread_spin_lock(&shared_data.spinlock);     /* 阻塞等待自旋锁 */
        ret = ++shared_data.value;
        pthread_spin_unlock(&shared_data.spinlock);

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
        pthread_spin_lock(&shared_data.spinlock);     /* 阻塞等待自旋锁 */
        ret = ++shared_data.value;
        pthread_spin_unlock(&shared_data.spinlock);

        printf("thread_2 value = [%d]\r\n", ret);
        if (shared_data.value >= 12)
            break;
        sleep(1);
    }
    
    pthread_exit((void*)&ret);
}

int main(int argc, char* args[])
{
    pthread_t tid1;
    pthread_t tid2;
    int* ret;

    /* 自旋锁spin：与mutex类似，但自旋锁不是通过休眠使线程阻塞，而是在获取锁之前一
                   直处于忙等(自旋)阻塞状态，做无用功占用CPU。自旋锁可用于以下情况：锁被持有的时间短，
                   且线程并不希望在重新调度上费太多成本。
       pthread_spin_init       初始化自旋锁 PTHREAD_PROCESS_PRIVATE PTHREAD_PROCESS_SHARED
       pthread_spin_lock       阻塞待待自旋锁(线程不睡眠，可以理解成浪费CPU资源)，return 0表时已加锁
       pthread_spin_trylock    非阻塞锁定自旋锁，return 0表时已加锁
       pthread_spin_unlock     解锁自旋锁
       pthread_spin_destroy    回收自旋锁 */

    pthread_spin_init(&shared_data.spinlock, PTHREAD_PROCESS_PRIVATE);      /* 初始化自旋锁 */
    shared_data.value = 0;

    pthread_create(&tid1, 0, thread_1, (void*)0);
    pthread_create(&tid2, 0, thread_2, (void*)0);
    pthread_join(tid1, (void*)&ret);    printf("thread_1 return [%d]\r\n", *ret);
    pthread_join(tid2, (void*)&ret);    printf("thread_2 return [%d]\r\n", *ret);

    pthread_spin_destroy(&shared_data.spinlock);      /* 回收自旋锁 */
           
    exit(0);
}



