#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <wait.h>


int glob = 6;
char buf[] = "a write to stdout\r\n";

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


    pid = fork();
    if (pid < 0)
        printf("fork error\r\n");
    else if (pid > 0)
    {
        // parent process
        if (waitpid(pid, &status, 0) != pid)    // wait first child process
            printf("wait error\r\n");
        pr_exit(status);
    }
    else if (pid == 0)
    {
        // first child process
        pid = fork();
        if (pid < 0)
            printf("fork error\r\n");
        else if (pid > 0)
        {
            // first child process
            exit(5);    // stop
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



