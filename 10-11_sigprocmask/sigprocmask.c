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
        // �û����ն��ϰ��˳���(һ�����Ctrl+\)
        printf("received signal: %s\r\n", STR(SIGQUIT));
        
        signal(SIGQUIT, SIG_DFL);   // Ĭ����ֹ����
        //signal(SIGQUIT, SIG_IGN);   // ���ԣ�������
    }
    else
        printf("received signal: %d\r\n", signo);

    return;
}

int main(int argc, char* args[])
{
    /* sigprocmask
     * �����ź����μ�
     * how:
     *      SIG_BLOCK:   set����������ϣ�������ĸ����ź�
     *      SIG_UNBLOCK: set����������ϣ������������ź�
     *      SIG_SETMASK: setΪ�ý����µ��ź����μ�
     * set:
     * oldset:           �����ϵ����μ�
     * ����/����: ����֮���ٲ�����ָ���źŶ�������(�ź�δ��״̬)��
     *            ���������ý��̣�ֱ�����źŲ��ٱ�������
     *            ���ǽ������/���� �� ����ָ���ź�Ϊ SIG_IGN(����)
     */
    //int sigprocmask(int how, sigset_t *set, sigset_t *oldset)


    sigset_t new_mask;
    sigset_t old_mask;
    sigset_t pend_status;

    signal(SIGQUIT, sig_slot);
        
    sigemptyset(&new_mask);         // ����źż�
    //sigfillset(&new_mask);          // �źż�ȫΪ����
    sigaddset(&new_mask, SIGQUIT);  // ���� SIGQUIT
    //sigdelset(&new_mask, SIGQUIT);  // ��� SIGQUIT


    // �ź����μ� ����SIGQUIT
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);
    printf("%s is locked\r\n", STR(SIGQUIT));
    sleep(5);

    // ��ѯ�ź�δ��״̬��(�ź��Ѳ���������������δ���͵�����)
    sigpending(&pend_status);     
    printf("\r\n%s pending status: %s\r\n", STR(SIGQUIT),
           sigismember(&pend_status, SIGQUIT) ? "1":"0");   // ���ָ���ź�λ�Ƿ�������

#if 1   // �ź����μ� ��ԭ�ϵ����μ���SIGQUIT������Σ�δ����SIGQUIT�����������������͵�������
    sigprocmask(SIG_SETMASK, &old_mask, 0);
#else
    sigprocmask(SIG_UNBLOCK, &new_mask, 0);
#endif
    printf("%s is unlocked\r\n", STR(SIGQUIT));
    sleep(5);
    
    /*
        ./sigprocmask
        SIGQUIT is locked               // ��ӡ�������ݺ�
        ^\^\                            // ��2��Ctrl+\ ����SIGQUIT
        SIGQUIT pending status: 1
        received signal: SIGQUIT
        SIGQUIT is unlocked             // ��ӡ�������ݺ�
        ^\Quit (core dumped)            // ��1��Ctrl+\ ����SIGQUIT,���������˳���SIGQUITĬ�ϴ���Ϊ��ֹ����
     */
    
    exit(0);
}



