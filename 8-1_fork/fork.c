#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/resource.h>


int glob = 6;
char buf[] = "a write to stdout\r\n";

int main(int argc, char* args[])
{
    int var;
    pid_t pid;

    var = 88;

    write(STDOUT_FILENO, buf, sizeof(buf) - 1);

    printf("fork befor[getpid = %d]\r\n", getpid());
    fflush(fdopen(STDOUT_FILENO, "a+"));    // flush now
    pid = fork();   // create copy process
    printf("fork after[getpid = %d]\r\n", getpid());

    if (pid < 0)
        printf("fork error\r\n");
    else if (pid > 0)
    {
        // parent process
        sleep(2);
    }
    else if (pid == 0)
    {
        // child process
        glob++;
        var++;
    }

    printf(" getpid = %d,"
           " getppid = %d,"
           " glob = %d,"
           " var = %d\r\n", getpid(), getppid(), glob, var);

    /*
     ./fork
     ./fork > temp.out;cat temp.out
     */

    exit(0);
}



