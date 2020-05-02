#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

extern char** environ;

int main(int argc, char* args[])
{

    // all en params
    printf("...................................................................\r\n");
    while (*environ)
    {
        printf("%s\r\n", *environ);
        environ++;
    }
    printf("...................................................................\r\n");


    //printf("[PATH] = %s\r\n", getenv("PATH"));
    /*
     *  getenv   putenv
     */

    exit(0);
    return 0;
}



