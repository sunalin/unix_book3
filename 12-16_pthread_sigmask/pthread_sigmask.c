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
    pthread_mutex_t     mutex;
    pthread_cond_t      cond;
}MY_DATA;
MY_DATA quit_flag =
{
    .value  = 0,
    .mutex  = PTHREAD_MUTEX_INITIALIZER,
    .cond   = PTHREAD_COND_INITIALIZER,
};


void* thread_signal(void* arg)
{
    pthread_detach(pthread_self());
    printf("thread_signal run...\r\n");
    static int ret = 0;
    sigset_t mask = *((sigset_t*)arg);
    int signo = 0;
    
    while (1)
    {
        sigwait(&mask, &signo);
        printf("recv signal: %2d[%s]\r\n", signo, strsignal(signo));
        switch (signo)
        {
        case SIGINT:    /* ctrl+c */
            break;
        case SIGQUIT:   /* ctrl+\ */
            {
                pthread_mutex_lock(&quit_flag.mutex);
                quit_flag.value = 1;    /* �����߳��˳� */
                pthread_mutex_unlock(&quit_flag.mutex);            
                pthread_cond_signal(&quit_flag.cond);
            }
            break;
        default:
            break;
        }
        signo = 0;
    }
    
    pthread_exit((void*)&ret);
}

int main(int argc, char* args[])
{
    sigset_t mask;
    sigset_t oldmask;
    pthread_t tid1;

    /* pthread_sigmask    ���õ�ǰ���źų������֣�ÿ���߳̾����Լ����ź�
       �����֣�ʹ��pthread_create�����̣߳����̻߳�̳е�ǰ�̵߳��źų���
       ���֣����̱߳���ʹ��pthread_sigmask�����̵߳Ľ�����û�ж�
       ��sigprocmask����Ϊ

       sigwait(set,signo)    �����ȴ�setָ����һ������δ��״̬�ź�
       �����������̵߳���sigwait֮ǰҪ������ָ���źţ�sigwait����֮ǰ
       ����ɾ����δ���ź�(�������Խ���Ĭ�Ϸ�ʽ�����signalָ���Ĵ���
       ����)���������ź�ͨ��signo���أ����첽�������ź���ͬ����ʽ����
       һ���������ź�ֻ�ܱ�����һ�Σ�����߳�û�д������ǻ��Խ���
       Ĭ�Ϸ�ʽ����

       һ�����sigwait/sigwaitinfo/sigtimedwait���⼸������������߳�
       �źţ�һ��ʹ��һ���̼߳��д����ź�

       kill    ���źŸ�����
       kill(pid,0)==[ESRCH ���̲�����][0 ���̴���]
       
       pthread_kill    ���źŸ��������е������߳�
       pthread_kill(threadid,0)==[ESRCH �̲߳�����][0 �̴߳���]
       pthread_sigqueue  ���pthread_kill�������ṩ�źŸ�����Ϣ��sigwaitinfo */


    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    pthread_sigmask(SIG_SETMASK, &mask, &oldmask);          /* ���õ�ǰ�߳��ź������� */
    pthread_create(&tid1, 0, thread_signal, (void*)&mask);  /* �̳е�ǰ�߳��ź������� */

    kill(getpid(), SIGINT);
    pthread_kill(tid1, SIGINT);

    /* �ȴ��˳� */
    pthread_mutex_lock(&quit_flag.mutex);
    while (quit_flag.value == 0)
        pthread_cond_wait(&quit_flag.cond, &quit_flag.mutex);
    quit_flag.value = 0;
    pthread_mutex_unlock(&quit_flag.mutex);
    printf("main exit...\r\n");


    sigprocmask(SIG_SETMASK, &oldmask, 0);
    
    return 0;
}



