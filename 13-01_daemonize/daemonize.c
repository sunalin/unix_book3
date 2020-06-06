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

#include <stropts.h>
#include <sys/mman.h>
#include <syslog.h>



int main(int argc, char* args[])
{
    /* �����ػ����� */
    int fd_stdin;
    int fd_stdout;
    int fd_stderr;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    /* ������ļ��������������� */
    umask(0);
    /* ��ȡϵͳ֧�ֵ�����ļ�������ֵ */
    getrlimit(RLIMIT_NOFILE, &rl);  

    /* �����»Ự����Ϊ�»Ự���׽��� */
    pid = fork();
    if (pid > 0)
        exit(0);    /* parent process */
    setsid();


    /* ��Ϊ���׽��̣���ֹ��ȡ�Ŀ����ն�
       SIGHUP�ն����ӶϿ�������Ĭ�ϴ�������ֹ���� */
    sa.sa_flags = 0;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGHUP, &sa, 0);
    pid = fork();
    if (pid > 0)
        exit(0);    /* parent process */

    /* ����·����Ϊ��Ŀ¼ */
    chdir("/");

    /* �ر������ļ������� */
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (int i = 0; i < rl.rlim_max; i++)
        close(i);
    fd_stdin = open("/dev/null", O_RDWR);   /* STDIN_FILENO  >> /dev/null*/
    fd_stdout = dup(0);                     /* STDOUT_FILENO >> /dev/null*/
    fd_stderr = dup(0);                     /* STDERR_FILENO >> /dev/null*/

    /* log */
    openlog(args[0], LOG_CONS, LOG_DAEMON);
    if (fd_stdin  != STDIN_FILENO ||
        fd_stdout != STDOUT_FILENO ||
        fd_stderr != STDERR_FILENO)
    {
        syslog(LOG_ERR, "fd_stdin[%d] fd_stdin[%d] fd_stdin[%d]\n",
               fd_stdin, fd_stdout, fd_stderr);
        exit(1);
    }

    /* ������ػ�����Ҫ��������� */
    while (1)
    {
        sleep(1);
    }

    /*
       ./daemonize
       ps -efj
       ps -efj|grep daemonize
       kill 3095
     */

    return 0;
}



