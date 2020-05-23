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


void slot_sig(int signo, siginfo_t* info, void* ctx)
{
    // �������ַ�ʽ���ܻ��sigqueue����������
    //printf("info->si_int = %d\r\n", info->si_int);
    printf("info->si_value.sival_int = %d\r\n", info->si_value.sival_int);
}

int main(int argc, char* args[])
{
    /* sigqueue    �����źţ���kill���ƣ���sigqueue���Դ��ݸ�����Ϣ���������
       ����Ŀǰlinux���ź�֧���Ŷӣ��������г���ȡ����ϵͳ
       ��ʵʱ�ź�(��֧���Ŷ�)�� С��SIGRTMIN
       ʵʱ�ź�(֧���Ŷ�)��SIGRTMIN ~ SIGRTMAX */

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;  // ��ʹ�� sa_sigaction
    act.sa_sigaction = slot_sig;
#define sig_num         (SIGINT)    // ��ʵʱ�ź�
#define sig_num_real    (SIGRTMIN)  // ʵʱ�ź�
    sigaction(sig_num, &act, 0);
    sigaction(sig_num_real, &act, 0);
    printf(" SIGRTMIN = %d \r\n SIGRTMAX = %d\r\n", SIGRTMIN, SIGRTMAX);

    sigset_t set;
    sigset_t old;
    sigemptyset(&set);
    sigaddset(&set, sig_num);
    sigaddset(&set, sig_num_real);
    sigprocmask(SIG_BLOCK, &set, &old);

    /* �������ź������ڼ����յ���ͬʵʱ�źű��Ŷӣ�����յ���ͬ��ʵʱ�źŲ��Ŷ�(ֻ����һ��)��������κ����δ����ź� */
    union sigval val;
    val.sival_int = 1;      sigqueue(getpid(), sig_num, val);
    val.sival_int = 2;      sigqueue(getpid(), sig_num, val);
    val.sival_int = 3;      sigqueue(getpid(), sig_num, val);
    val.sival_int = 11;     sigqueue(getpid(), sig_num_real, val);
    val.sival_int = 12;     sigqueue(getpid(), sig_num_real, val);
    val.sival_int = 13;     sigqueue(getpid(), sig_num_real, val);

    sigprocmask(SIG_SETMASK, &old, 0);

    // https://www.cnblogs.com/subo_peng/p/5325326.html
    
    exit(0);
}



