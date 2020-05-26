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
                quit_flag.value = 1;    /* 让主线程退出 */
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

    /* pthread_sigmask    设置当前线信号程屏蔽字，每个线程均有自己的信号
       屏蔽字，使用pthread_create创建线程，子线程会继承当前线程的信号程屏
       蔽字，多线程必须使用pthread_sigmask，多线程的进程中没有定
       义sigprocmask的行为

       sigwait(set,signo)    阻塞等待set指定的一个或多个未决状态信号
       发生，所以线程调用sigwait之前要先阻塞指定信号，sigwait返回之前
       会先删除该未决信号(不会用以进程默认方式处理或signal指定的处理
       程序)，发生的信号通过signo返回，对异步产生的信号以同步方式处理，
       一个产生的信号只能被处理一次，如果线程没有处理，则还是会以进程
       默认方式处理

       一般采用sigwait/sigwaitinfo/sigtimedwait等这几个函数处理多线程
       信号，一般使用一个线程集中处理信号

       kill    发信号给进程
       kill(pid,0)==[ESRCH 进程不存在][0 进程存在]
       
       pthread_kill    发信号给本进程中的其它线程
       pthread_kill(threadid,0)==[ESRCH 线程不存在][0 线程存在]
       pthread_sigqueue  相比pthread_kill，可以提供信号附加信息，sigwaitinfo */


    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    pthread_sigmask(SIG_SETMASK, &mask, &oldmask);          /* 设置当前线程信号屏蔽字 */
    pthread_create(&tid1, 0, thread_signal, (void*)&mask);  /* 继承当前线程信号屏蔽字 */

    kill(getpid(), SIGINT);
    pthread_kill(tid1, SIGINT);

    /* 等待退出 */
    pthread_mutex_lock(&quit_flag.mutex);
    while (quit_flag.value == 0)
        pthread_cond_wait(&quit_flag.cond, &quit_flag.mutex);
    quit_flag.value = 0;
    pthread_mutex_unlock(&quit_flag.mutex);
    printf("main exit...\r\n");


    sigprocmask(SIG_SETMASK, &oldmask, 0);
    
    return 0;
}



