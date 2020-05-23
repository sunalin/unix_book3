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
    // 以下两种方式都能获得sigqueue发来的数据
    //printf("info->si_int = %d\r\n", info->si_int);
    printf("info->si_value.sival_int = %d\r\n", info->si_value.sival_int);
}

int main(int argc, char* args[])
{
    /* sigqueue    发送信号，与kill类似，但sigqueue可以传递附加信息给处理程序
       对于目前linux，信号支持排队，至于排列长度取决于系统
       非实时信号(不支持排队)： 小于SIGRTMIN
       实时信号(支持排队)：SIGRTMIN ~ SIGRTMAX */

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;  // 可使用 sa_sigaction
    act.sa_sigaction = slot_sig;
#define sig_num         (SIGINT)    // 非实时信号
#define sig_num_real    (SIGRTMIN)  // 实时信号
    sigaction(sig_num, &act, 0);
    sigaction(sig_num_real, &act, 0);
    printf(" SIGRTMIN = %d \r\n SIGRTMAX = %d\r\n", SIGRTMIN, SIGRTMAX);

    sigset_t set;
    sigset_t old;
    sigemptyset(&set);
    sigaddset(&set, sig_num);
    sigaddset(&set, sig_num_real);
    sigprocmask(SIG_BLOCK, &set, &old);

    /* 例：在信号屏蔽期间多次收到相同实时信号被排队，多次收到相同非实时信号不排队(只处理一次)，解除屏蔽后依次处理信号 */
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



