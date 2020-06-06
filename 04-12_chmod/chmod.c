#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


int main(int argc, char* args[])
{
#define EXEC_MASK   S_IRWXU|\
                    S_IRWXG|\
                    S_IRWXO

#define MASK1       S_IRUSR|S_IWUSR|\
                    S_IRGRP|S_IWGRP|\
                    S_IROTH|S_IWOTH
    umask(0);   // 不屏蔽
    int fd = open("./foo1", O_WRONLY|O_CREAT|O_TRUNC, EXEC_MASK);
    close(fd);

    /* chmod 更改文件权限 */
    chmod("./foo1", MASK1);
    /*
        ./umask
        ls -hal
     */
    
    exit(0);
}



