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


#define STR(sig)    #sig

void pr_mask(const char* str)
{
    sigset_t sigset;
    int errno_save;

    errno_save = errno;
    sigprocmask(0, 0, &sigset);
    printf("\r\n%s", str ? str:"");
    if (sigismember(&sigset, SIGINT))       printf("%s ", STR(SIGINT));
    if (sigismember(&sigset, SIGQUIT))      printf("%s ", STR(SIGQUIT));
    if (sigismember(&sigset, SIGUSR1))      printf("%s ", STR(SIGUSR1));
    if (sigismember(&sigset, SIGUSR2))      printf("%s ", STR(SIGUSR2));
    if (sigismember(&sigset, SIGALRM))      printf("%s ", STR(SIGALRM));
    if (sigismember(&sigset, SIGCONT))      printf("%s ", STR(SIGCONT));
    // .....
    printf("\r\n");
    errno = errno_save;
}

static void sig_SIGINT(int signo)
{
    pr_mask("sig_SIGINT [start] mask: ");
    return;
}

int main(int argc, char* args[])
{
    sigset_t new_mask;
    sigset_t old_mask;
    sigset_t wait_mask;
    
    signal(SIGINT, sig_SIGINT);     // ctrl+c

    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGINT);
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);   // SIGINT  加入屏蔽字

    // 临界区
    pr_mask("critical [start] mask: ");
    printf("critical do something......");
    pr_mask("critical [end]   mask: ");

    /* sigsuspend    使进程挂起（进入睡眠状态）等待信号的中断，信号屏蔽字设为sigmask集
       (sigmask为空集则sigsuspend和pause是一样的，可以接受任何信号的中断)，
       如果没有信号发生，进程会一直挂起，当有信号发生时(该信号不能是sigmask屏蔽的信号)，
       执行该信号处理程序，处理程序返回后sigsuspend才返回，并且信号屏蔽字恢复为sigsuspend
       调用之前的状态

       需要了解：正常情况下，执行一个信号的处理程序时，该信号自动加入信号屏蔽字，处理程序
       返回时，该信号自动解除屏蔽 */
    //int sigsuspend(const sigset_t* sigmask);

    sigemptyset(&wait_mask);
    sigaddset(&wait_mask, SIGUSR1);     // SIGUSR1  加入屏蔽字
    printf("sigsuspend [SIGUSR1]\r\n");
    sigsuspend(&wait_mask);
    pr_mask("sigsuspend after mask: ");


    //
    sigprocmask(SIG_SETMASK, &old_mask, 0);

    exit(0);
}



