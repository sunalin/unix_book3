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
    if (signo == SIGUSR1)
    {
        // kill -USR1 4097    // 向4097进程发送信号 SIGUSR1
        printf("sig_slot received signal[%s]\r\n", STR(SIGUSR1));
    }
    else if (signo == SIGUSR2)
    {
        // kill -USR2 4097    // 向4097进程发送信号 SIGUSR2
        printf("sig_slot received signal[%s]\r\n", STR(SIGUSR2));
    }
    else if (signo == SIGTERM)
    {
        // kill 4097          // 向4097进程发送信号 SIGTERM
        // 如果进程未处理该信号，则默认处理就是 终止程序动行
        printf("sig_slot received signal[%s]\r\n", STR(SIGTERM));
        exit(0);
    }
    else if (signo == SIGINT)
    {
        // 用户按中断键(一般采用DELETE或Ctrl+C)
        printf("sig_slot received signal[%s]\r\n", STR(SIGINT));
        exit(0);
    }    
    else if (signo == SIGQUIT)
    {
        // 用户在终端上按退出键(一般采用Ctrl+\)
        printf("sig_slot received signal[%s]\r\n", STR(SIGQUIT));
        exit(0);
    }
    else
        printf("sig_slot received signal[%d]\r\n", signo);

    return;
}

int main(int argc, char* args[])
{
    // 应用程序无法被捕捉或忽略 SIGKILL SIGSTOP 这两个信号

    signal(SIGUSR1, sig_slot);
    signal(SIGUSR2, sig_slot);
    signal(SIGTERM, sig_slot);
    if (signal(SIGINT,  SIG_IGN) != SIG_IGN)
        signal(SIGINT, sig_slot);
    if (signal(SIGQUIT,  SIG_IGN) != SIG_IGN)
        signal(SIGQUIT, sig_slot);
        
    while (1)
        pause();    // 进程睡眠

    /*
    ./signal_SIGUSR1_SIGUSR2 &      // 在后台启动进程,shell自动将后台进程中SIGINT与SIGQUIT处理方式设置为忽略
    [1] 4097                        // 执行上条命令后 shell打印该进程作业号和进程PID
    kill -USR1 4097
    kill -USR2 4097
    kill 4097
     */
    
    exit(0);
}



