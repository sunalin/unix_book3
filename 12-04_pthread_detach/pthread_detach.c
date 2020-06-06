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

    /* 分离线程：不关心线程的返回值状态时，可使用分离线程(指定detach属性)，
                 这样线程终止后自动回收资源，对分离线程不能使用 pthread_join 

       分离线程方法：
           1、调用pthread_detach(pthread_t)，可由其它线程调用，也可由线程自已调用
           2、pthread_create 第2个参数 attr 指定 PTHREAD_CREATE_DETACHED 属性 */

    printf("main run...\r\n");

    pthread_create(&tid1, 0, thread_1, (void*)0);
    pthread_detach(tid1);
    
    pthread_attr_init(&attr2);
    pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_DETACHED);
    pthread_create(&tid2, &attr2, thread_2, (void*)0);

    sleep(1);   // 让子线程有执行的机会
    
    exit(0);
}



