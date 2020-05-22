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

static sigjmp_buf               jmpbuf;
static volatile sig_atomic_t    canjump;

static void sig_SIGUSR1(int signo)
{
    time_t starttime;

    if (canjump == 0)
        return;

    pr_mask("sig_SIGUSR1 [start] mask: ");
    alarm(3);
    starttime = time(0);
    while (1)
    {
        if (time(0) >= starttime + 5)
            break;
    }
    pr_mask("sig_SIGUSR1 [end]   mask: ");

    canjump = 0;
    siglongjmp(jmpbuf, 1);
}

static void sig_SIGALRM(int signo)
{
    pr_mask("sig_SIGALRM mask: ");
}

int main(int argc, char* args[])
{
    /* siglongjmp时会恢复之前sigsetjmp调用时保存的信号屏敝字
       当调用一个信号处理程序时，被捕捉到的信号加到进程的当前信号屏蔽字。
       当从信号处理程序返回时，原来的屏蔽字被恢复 */

    // signal(..) 里面会调用 sigaction(...)
    signal(SIGUSR1, sig_SIGUSR1);
    signal(SIGALRM, sig_SIGALRM);

    pr_mask("main [start] mask: ");
    if (sigsetjmp(jmpbuf, 1))
    {
        pr_mask("main [end]   mask: ");
        exit(0);
    }

    canjump = 1;
    while (1)
        pause();

    /*
       ./siglongjmp &
       [1] 2376
       kill -SIGUSR1 2376
     */

    exit(0);
}



