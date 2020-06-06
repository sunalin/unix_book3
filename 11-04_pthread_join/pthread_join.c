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


void cleanup(void* arg)
{
    printf("[%s] cleanup\r\n", arg ? ((char*)arg) : "");
}

void* thread_1(void* arg)
{
    static int ret_val = 0;
    ret_val = 5;
    printf("thread_1 run...  ret_val[%d] &ret_val[0x%lx]\r\n", ret_val, (long)&ret_val);

    pthread_cleanup_push(cleanup, "thread_1 c1");
    pthread_cleanup_pop(1);
    pthread_cleanup_push(cleanup, "thread_1 c2");
    pthread_cleanup_pop(0);

    return (void*)&ret_val;
}

void* thread_2(void* arg)
{
    static int ret_val = 0;
    ret_val = 6;
    printf("thread_2 run...  ret_val[%d] &ret_val[0x%lx]\r\n", ret_val, (long)&ret_val);

    pthread_cleanup_push(cleanup, "thread_2 c1");
    pthread_cleanup_pop(0);
    pthread_cleanup_push(cleanup, "thread_2 c2");
    pthread_cleanup_pop(1);

    pthread_exit(&ret_val);
}

int main(int argc, char* args[])
{
    pthread_t tid1;
    pthread_t tid2;
    int* ret;

    pthread_create(&tid1, 0, thread_1, (void*)0);
    pthread_create(&tid2, 0, thread_2, (void*)0);

    /* pthread_join    �����ȴ��߳���ֹ��������Դ������retָ��=pthread_exit�ش���ָ�����
       pthread_cleanup_push��pthread_cleanup_pop��Ҫ�ɶԴ��ڣ�����ע���߳�����
       ����(return��pthread_exit����)��pthread_cleanup_pop�Ĳ�������ʱ����Ч */
    pthread_join(tid1, (void*)&ret);
    printf("thread_1 return  *ret[%d] ret[0x%lx]\r\n", *ret, (long)ret);
    pthread_join(tid2, (void*)&ret);
    printf("thread_2 return  *ret[%d] ret[0x%lx]\r\n", *ret, (long)ret);

           
    exit(0);
}



