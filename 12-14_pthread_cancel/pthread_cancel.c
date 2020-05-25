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
    static int ret = 0;

    /* 可以在任意时间点被取消 pthread_cancel */
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
    while (1)
        sleep(1);
    
    pthread_exit((void*)&ret);
}

void* thread_2(void* arg)
{
    pthread_detach(pthread_self());
    printf("thread_2 run...\r\n");
    static int ret = 0;

    /* 可以在任意时间点被取消 pthread_cancel */
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
    while (1)
        sleep(1);
    
    pthread_exit((void*)&ret);
}

int main(int argc, char* args[])
{
    pthread_t tid1;
    pthread_t tid2;

    /* pthread_cancel    发起取消请求，该调用不会阻塞，只是发出一个取消请求而已(具体
       不会不取消由两个线程属性决定：可取消状态属性、取消类型属性)

       pthread_setcancelstate    设置可取消状态属性(相当于是否支持被取消的开关)
       1、PTHREAD_CANCEL_ENABLE(默认)：线程会处理取消请求
       2、PTHREAD_CANCEL_DISABLE：线程不处理取消请求，取消请求被挂起(取消请求仍然保留，
       重新设为ENABLE时，该取消请求将被处理)

       pthread_setcanceltype    设置取消类型属性(决定处理取消请求的方式，在PTHREAD_CANCEL_ENABLE时才有意义)
       1、PTHREAD_CANCEL_DEFERRED 推迟取消(默认)：在下一个取消点取消
       2、PTHREAD_CANCEL_ASYNCHRONOUS 异步取消：在任意时间取消，不用等到取消点

       pthread_testcancel    手动添加一个取消点(不止这个函数，还有其它的)

       pthread_kill(threadid,sig)    给线程发信号
       sig=0是个保留信号，用返回值判断线程是不是还活着(0 活着;ESRCH 线程不存在;EINVAL 信号不合法) */
    
    pthread_create(&tid1, 0, thread_1, (void*)0);
    pthread_create(&tid2, 0, thread_2, (void*)0);

    int i = 0;
    while (1)
    {
        if (i == 2)
            pthread_cancel(tid1);
        else if (i == 4)
            pthread_cancel(tid2);
        
        printf("thread id: %lx [%s]\t\t"
               "thread id: %lx [%s]\r\n",
               tid1, pthread_kill(tid1, 0) == ESRCH ? "die" : "alive",
               tid2, pthread_kill(tid2, 0) == ESRCH ? "die" : "alive");

        i++;
        if (i >= 7)
            break;
        sleep(1);
    }
    
    return 0;
}



