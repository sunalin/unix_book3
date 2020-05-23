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
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);   // SIGINT  ����������

    // �ٽ���
    pr_mask("critical [start] mask: ");
    printf("critical do something......");
    pr_mask("critical [end]   mask: ");

    /* sigsuspend    ʹ���̹��𣨽���˯��״̬���ȴ��źŵ��жϣ��ź���������Ϊsigmask��
       (sigmaskΪ�ռ���sigsuspend��pause��һ���ģ����Խ����κ��źŵ��ж�)��
       ���û���źŷ��������̻�һֱ���𣬵����źŷ���ʱ(���źŲ�����sigmask���ε��ź�)��
       ִ�и��źŴ�����򣬴�����򷵻غ�sigsuspend�ŷ��أ������ź������ָֻ�Ϊsigsuspend
       ����֮ǰ��״̬

       ��Ҫ�˽⣺��������£�ִ��һ���źŵĴ������ʱ�����ź��Զ������ź������֣��������
       ����ʱ�����ź��Զ�������� */
    //int sigsuspend(const sigset_t* sigmask);

    sigemptyset(&wait_mask);
    sigaddset(&wait_mask, SIGUSR1);     // SIGUSR1  ����������
    printf("sigsuspend [SIGUSR1]\r\n");
    sigsuspend(&wait_mask);
    pr_mask("sigsuspend after mask: ");


    //
    sigprocmask(SIG_SETMASK, &old_mask, 0);

    exit(0);
}



