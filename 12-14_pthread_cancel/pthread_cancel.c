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

    /* ����������ʱ��㱻ȡ�� pthread_cancel */
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

    /* ����������ʱ��㱻ȡ�� pthread_cancel */
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

    /* pthread_cancel    ����ȡ�����󣬸õ��ò���������ֻ�Ƿ���һ��ȡ���������(����
       ���᲻ȡ���������߳����Ծ�������ȡ��״̬���ԡ�ȡ����������)

       pthread_setcancelstate    ���ÿ�ȡ��״̬����(�൱���Ƿ�֧�ֱ�ȡ���Ŀ���)
       1��PTHREAD_CANCEL_ENABLE(Ĭ��)���̻߳ᴦ��ȡ������
       2��PTHREAD_CANCEL_DISABLE���̲߳�����ȡ������ȡ�����󱻹���(ȡ��������Ȼ������
       ������ΪENABLEʱ����ȡ�����󽫱�����)

       pthread_setcanceltype    ����ȡ����������(��������ȡ������ķ�ʽ����PTHREAD_CANCEL_ENABLEʱ��������)
       1��PTHREAD_CANCEL_DEFERRED �Ƴ�ȡ��(Ĭ��)������һ��ȡ����ȡ��
       2��PTHREAD_CANCEL_ASYNCHRONOUS �첽ȡ����������ʱ��ȡ�������õȵ�ȡ����

       pthread_testcancel    �ֶ����һ��ȡ����(��ֹ�������������������)

       pthread_kill(threadid,sig)    ���̷߳��ź�
       sig=0�Ǹ������źţ��÷���ֵ�ж��߳��ǲ��ǻ�����(0 ����;ESRCH �̲߳�����;EINVAL �źŲ��Ϸ�) */
    
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



