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

int main(int argc, char* args[])
{
    printf("print......[real uit = %d, effective uid = %d]\r\n",
           getuid(),
           geteuid());

    exit(0);
}



