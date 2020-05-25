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
    pthread_key_t   key;
    pthread_once_t  once;
}MY_DATA;
MY_DATA private_data = {.once = PTHREAD_ONCE_INIT,};

void buf_destroy(void* obj)
{
    printf("thread id:%lx \t free(%p)\r\n", pthread_self(), obj);
    free(obj);
}

void buf_key_init(void)
{
    /* pthread_key_create 创建一个线程私有数据key，并指定key的析构函数，创建
       key后所有线程都可以使用该key，每线程可根据自己的需要让key关联不同的数
       据对象，相当于提供了一个同名而不同值的全局变量(一键多值)，key相当于变
       量名，每个线程创建时关联数据对象是空值0，通常与malloc关联

       pthread_setspecific(key)     设置当前线程私有数据对象
       pthread_getspecific(key)     获取当前线程私有数据对象
       pthread_key_delete(key)      取消key与线程私有数据的关联，不会激活析构函数，提前手动free
       
       当线程pthread_exit或返回时，如果(key关联数据对象值非空时)则(以key关联数据对象为
       参数)调用析构函数，值为空时不会调用析构函数，exit或其它异常终止时不会调用析构函数
       注：当key关联数据对象为malloc分配的内存时，可以合理使用析构函数free */
    pthread_key_create(&private_data.key, buf_destroy);    
    printf("pthread_key_create\r\n");
}

void buf_init(void)
{
    /* 在多个线程调用pthread_once时(针对同一个pthread_once_t)，
       pthread_once指定的函数只会被调用一次 */
    pthread_once(&private_data.once, buf_key_init);      /* 只会调用一次buf_key_init */

    char* tmp = pthread_getspecific(private_data.key);   /* 获取key关联数据对象 */
    if (tmp == 0)   /* 每个线程创建时关联对象是空值0 */
    {
        tmp = malloc(256); /* 指向malloc内存，也可以指向其它地址 */
        if (tmp)
            pthread_setspecific(private_data.key, tmp);  /* 设置key关联数据对象 */
        printf("thread id:%lx \t malloc = %p\r\n", pthread_self(), tmp);
    }
}

void* buf_get(void)
{
    return pthread_getspecific(private_data.key);
}

void* thread_1(void* arg)
{
    pthread_detach(pthread_self());
    printf("thread_1 run...\r\n");
    static int ret = 0;

    buf_init();
    int* dat = buf_get();
    *dat = 100;
    while (1)
    {
        dat = buf_get();
        (*dat)++;

        printf("thread_1 dat = [%d]\r\n", *dat);
        if (*dat >= 110)
            break;
        sleep(1);
    }
    
    pthread_exit((void*)&ret);
}

void* thread_2(void* arg)
{
    pthread_detach(pthread_self());
    printf("thread_2 run...\r\n");
    static int ret = 0;

    buf_init();
    int* dat = buf_get();
    *dat = 10000;
    while (1)
    {
        dat = buf_get();
        (*dat)++;

        printf("thread_2 dat = [%d]\r\n", *dat);
        if (*dat >= 10010)
            break;
        sleep(1);
    }
    
    pthread_exit((void*)&ret);
}

int main(int argc, char* args[])
{
    pthread_t tid1;
    pthread_t tid2;

    pthread_create(&tid1, 0, thread_1, (void*)0);
    pthread_create(&tid2, 0, thread_2, (void*)0);

    buf_init();
    int* dat = buf_get();
    *dat = 0;
    while (1)
    {
        dat = buf_get();
        (*dat)++;

        printf("main dat = [%d]\r\n", *dat);
        if (*dat >= 13)
            break;
        sleep(1);
    }
    
    return 0;
}



