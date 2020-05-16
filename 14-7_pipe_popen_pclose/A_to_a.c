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
#include <ctype.h>

int main(int argc, char* args[])
{
    /*
       从标准输入读字符
       转成小写字符输出
     */
    int c;

    while (1)
    {
        c = getchar();
        if (c == EOF)
            break;

        if (isupper(c))
            c = tolower(c);

        if (putchar(c) == EOF)
            printf("A_to_a putchar error\r\n");
        if (c == '\n')
            fflush(stdout);
    }
    
    exit(0);
}



