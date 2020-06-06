#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *args[])
{
    /* getuid getgid */

    printf("uid: %d, gid: %d\r\n", getuid(), getgid());

    return 0;
}



