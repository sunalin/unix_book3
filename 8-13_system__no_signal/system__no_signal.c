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

#if 1
// 这里只是模仿system的实现，没有实现signal，注释此函数据将调用UNIX自带的system
// UNIX自带的system函数位于stdlib.h
int system(const char* cmdstring)
{
    pid_t pid;
    int status;

    if (!cmdstring)
        return 1;

    pid = fork();
    if (pid < 0)
        status = -1;
    else if (pid > 0)
    {
        // parent process        
        pid_t id = -1;
        while ((id = waitpid(pid, &status, 0)) < 0)
        {
            if (errno != EINTR)
            {
                status = -1;
                break;
            }
        }        
        printf("process end...[id = %d]\r\n", id);
    }
    else if (pid == 0)
    {
        // child process
        printf("process start...[getpid = %d]\r\n", getpid());
        execl("/bin/sh", "sh", "-c", cmdstring, (char*)0);
        exit(127);
    }

    return status;
}
#endif

int main(int argc, char* args[])
{
    int status = -1;

    //------------------
    status = system("date");
    if (status < 0)
        printf("system() error\r\n");
    pr_exit(status);

    //------------------
    status = system("nosuchcommand");
    if (status < 0)
        printf("system() error\r\n");
    pr_exit(status);

    //------------------
    status = system("ps; exit 44");
    if (status < 0)
        printf("system() error\r\n");
    pr_exit(status);

    exit(0);
}



