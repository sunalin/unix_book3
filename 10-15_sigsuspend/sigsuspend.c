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
    sigset_t zero_mask;
    
    signal(SIGINT, sig_SIGINT);     // ctrl+c

    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGINT);
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);   // SIGINT加入屏蔽字，不让SIGINT干扰临界区

    // 临界区
    pr_mask("critical [start] mask: ");
    printf("critical do something......");
    pr_mask("critical [end]   mask: ");

    /* 
       sigsuspend函数是pause函数的增强版。当sigsuspend函数的参数信号集
       为空信号集时，sigsuspend函数是和pause函数是一样的，可以接受任何信号的中断

       在sigsuspend函数调用时，会使进程挂起（进入睡眠状态）等待信号的中断，
       如果没有信号发生，进程会一直挂起，当有信号发生时，但该信号不是
       sigsuspend函数的信号集中所设置的屏蔽的信号时，sigsuspend会处理该信号，
       当该信号处理完成后，sigsuspend函数才返回，并执行接下来的代码
     */
    sigemptyset(&zero_mask);
    //sigaddset(&zero_mask, SIGQUIT);
    sigsuspend(&zero_mask);
    pr_mask("sigsuspend after mask: ");

    sigprocmask(SIG_SETMASK, &old_mask, 0);

    exit(0);
}



