#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <wait.h>


void pr_exit(int status)
{
    if (WIFEXITED(status))
        printf("child process normal termination, [exit(status)] status = %d\r\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("child process abnormal termination, signal number = %d%s\r\n", WTERMSIG(status), WCOREDUMP(status) ? "(core file.generated)" : "");
    else if (WIFSTOPPED(status))
        printf("child process stopped, signal number = %d\r\n", WSTOPSIG(status));
    else
        printf("pr_exit error\r\n");
}

int main(int argc, char* args[])
{
    pid_t pid;
    int status;
    /* waitpid    �ȴ��ӽ�����ֹ״̬,
       2��fork�Ա��⽩������(һ������ֹ�����丸����δ��������ƺ���Ľ��� wait() ) */

    pid = fork();
    if (pid > 0)
    {
        // parent process
        waitpid(pid, &status, 0);   // wait first child process
        pr_exit(status);
    }
    else if (pid == 0)
    {
        // first child process
        pid = fork();
        if (pid > 0)
        {
            // first child process
            exit(5);    // first child process exit
        }
        else if (pid == 0)
        {
            // second child process (parent == first child process)
            sleep(2);   // wait first child process stop
            printf("second child process, parent pid = %d\r\n", getppid());
            exit(0);
        }

    }

    exit(0);
}



