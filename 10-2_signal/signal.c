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
    if (signo == SIGUSR1)       /* kill -USR1 4097 */
    {
        printf("sig_slot received signal[%s]\r\n", STR(SIGUSR1));
    }
    else if (signo == SIGUSR2)  /* kill -USR2 4097 */
    {
        printf("sig_slot received signal[%s]\r\n", STR(SIGUSR2));
    }
    else if (signo == SIGTERM)  /* kill 4097 */
    {
        printf("sig_slot received signal[%s]\r\n", STR(SIGTERM));
        exit(0);
    }
    else if (signo == SIGINT)   /* Ctrl+C */
    {
        printf("sig_slot received signal[%s]\r\n", STR(SIGINT));
        exit(0);
    }    
    else if (signo == SIGQUIT)  /* Ctrl+\ */
    {
        printf("sig_slot received signal[%s]\r\n", STR(SIGQUIT));
        exit(0);
    }
    else
        printf("sig_slot received signal[%d]\r\n", signo);

    return;
}

int main(int argc, char* args[])
{
    // signal    Ӧ�ó����޷�����׽����� SIGKILL SIGSTOP �������ź�

    signal(SIGUSR1, sig_slot);
    signal(SIGUSR2, sig_slot);
    signal(SIGTERM, sig_slot);
    if (signal(SIGINT,  SIG_IGN) != SIG_IGN)
        signal(SIGINT, sig_slot);
    if (signal(SIGQUIT,  SIG_IGN) != SIG_IGN)
        signal(SIGQUIT, sig_slot);
        
    while (1)
        pause();        // ʹ���ý��̹���ֱ����׽��һ���ź�

    /*
       ./signal &       // shell����̨������SIGINT��SIGQUIT����ʽ����Ϊ����
       [1] 4097
       kill -USR1 4097
       kill -USR2 4097
       kill 4097
     */
    
    exit(0);
}



