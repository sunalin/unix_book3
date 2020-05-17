#include <stdio.h>
#include <string.h>
#include <unistd.h>
 
int main(int argc, char *args[])
{
    printf("current process id is:%d\r\n", getpid());

    return 0;
}



