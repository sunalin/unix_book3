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

int main(int argc, char* args[])
{
    char line[8192];
    FILE* fpin;

    /*
       popen ��forkһ���ӽ���,����pipe��һ��FILE*��ָ��
       "r":���� �൱���ӽ��̵ı�׼���
       "w":���� �൱���ӽ��̵ı�׼����
     */
    fpin = popen("./A_to_a", "r");    // 'A' ==>> 'a'
    if (fpin == 0)
    {
        printf("popen error\r\n");
        exit(0);
    }

    while (1)
    {
        fputs("please input>", stdout);
        fflush(stdout);

        if (fgets(line, sizeof(line), fpin) == NULL)    // read from pipe
            break;

        if (fputs(line, stdout) == EOF)
            printf("fputs error to pipe\r\n");
    }

    pclose(fpin);

    /*
       ./pipe_popen_pclose
     */

    exit(0);
}



