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

#define STR(sig)    #sig

static void sig_slot(int signo)
{
    if (signo == SIGQUIT)
    {
        // 用户在终端上按退出键(一般采用Ctrl+\)
        printf("received signal: %s\r\n", STR(SIGQUIT));
        
        signal(SIGQUIT, SIG_DFL);   // 默认终止程序
        //signal(SIGQUIT, SIG_IGN);   // 忽略，不处理
    }
    else
        printf("received signal: %d\r\n", signo);

    return;
}

int main(int argc, char* args[])
{
    /* sigprocmask    设置信号屏蔽集
       how:
            SIG_BLOCK:   set包含了我们希望阻塞的附加信号
            SIG_UNBLOCK: set包含了我们希望解除阻塞的信号
            SIG_SETMASK: set为该进程新的信号屏蔽集
       set:
       oldset:           返回老的屏蔽集
       阻塞/屏蔽: 设置之后再产生的指定信号都被阻塞(信号未决状态)，
                  不递送至该进程，直到该信号不再被阻塞，
                  除非解除阻塞/屏蔽 或 设置指定信号为 SIG_IGN(忽略) */
    //int sigprocmask(int how, sigset_t *set, sigset_t *oldset)

    sigset_t new_mask;
    sigset_t old_mask;
    sigset_t pend_status;

    signal(SIGQUIT, sig_slot);
        
    sigemptyset(&new_mask);         // 清空信号集
    sigaddset(&new_mask, SIGQUIT);  // 设置 SIGQUIT
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);   // 信号屏蔽集 增加SIGQUIT
    printf("%s is locked\r\n", STR(SIGQUIT));
    sleep(5);

    
    sigpending(&pend_status);   // 查询信号未决状态集(信号已产生但被阻塞，还未递送到进程)
    printf("pending status: [%s = %s]\r\n", STR(SIGQUIT), sigismember(&pend_status, SIGQUIT) ? "1":"0");

    // 信号屏蔽集 还原老的屏蔽集，SIGQUIT解除屏蔽，未决的SIGQUIT不再受阻塞而被递送到本进程
    sigprocmask(SIG_SETMASK, &old_mask, 0);
    //sigprocmask(SIG_UNBLOCK, &new_mask, 0);
    printf("%s is unlocked\r\n", STR(SIGQUIT));
    sleep(5);
    
    /*
        ./sigprocmask
        SIGQUIT is locked               // 打印这条数据后
        ^\^\                            // 按2次Ctrl+\ 产生SIGQUIT
        SIGQUIT pending status: 1
        received signal: SIGQUIT
        SIGQUIT is unlocked             // 打印这条数据后
        ^\Quit (core dumped)            // 按1次Ctrl+\ 产生SIGQUIT,程序立即退出，SIGQUIT默认处理为终止程序
     */
    
    exit(0);
}



