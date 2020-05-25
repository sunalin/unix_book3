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

int main(int argc, char* args[])
{
    int n;

    struct strbuf ctl;  char ctlbuf[8192];
    struct strbuf dat;  char datbuf[8192];
    int flag;

    ctl.buf = ctlbuf;
    ctl.maxlen = sizeof(ctlbuf);
    dat.buf = datbuf;
    dat.maxlen = sizeof(datbuf);

    while (1)
    {
        flag = 0;
        n = getmsg(STDIN_FILENO, &ctl, &dat, &flag);
        if (n < 0)
            printf("getmsg error\r\n");
        fprintf(stderr, "flag = %d, ctl.len = %d, dat.len = %d\r\n",
                flag, ctl.len, dat.len);
        if (dat.len == 0)
            exit(0);
        else if (dat.len > 0)
        {
            if (write(STDOUT_FILENO, dat.buf, dat.len) != dat.len)
                printf("write error\r\n");
        }
    }

    exit(0);
}



