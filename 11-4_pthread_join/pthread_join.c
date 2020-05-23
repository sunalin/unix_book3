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
    static int ret_val = 0;
    ret_val = 5;
    printf("thread_1 run...  ret_val[%d] &ret_val[0x%lx]\r\n", ret_val, (long)&ret_val);
    return (void*)&ret_val;
}

void* thread_2(void* arg)
{
    static int ret_val = 0;
    ret_val = 6;
    printf("thread_2 run...  ret_val[%d] &ret_val[0x%lx]\r\n", ret_val, (long)&ret_val);
    pthread_exit(&ret_val);
}

int main(int argc, char* args[])
{
    pthread_t tid1;
    pthread_t tid2;
    int* ret;

    pthread_create(&tid1, 0, thread_1, 0);
    pthread_create(&tid2, 0, thread_2, 0);

    /* pthread_join    阻塞等待线程终止，更改ret指向=pthread_exit回传的指针参数 */
    pthread_join(tid1, (void*)&ret);
    printf("thread_1 return  *ret[%d] ret[0x%lx]\r\n", *ret, (long)ret);
    pthread_join(tid2, (void*)&ret);
    printf("thread_1 return  *ret[%d] ret[0x%lx]\r\n", *ret, (long)ret);

           
    exit(0);
}



