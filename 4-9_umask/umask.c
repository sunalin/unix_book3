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

#define MASK2       S_IRUSR|S_IWUSR|\
                    S_IRGRP|S_IWGRP
    /*
       umask 进程设置 文件创建屏蔽字(读/写/可执行 权限)
       创建文件和目录都会用到屏蔽字
     */
    umask(0);   // 不屏蔽
    open("./foo1", O_WRONLY|O_CREAT|O_TRUNC, EXEC_MASK);
    umask(MASK1);
    open("./foo2", O_WRONLY|O_CREAT|O_TRUNC, EXEC_MASK);
    umask(MASK2);
    open("./foo3", O_WRONLY|O_CREAT|O_TRUNC, EXEC_MASK);
    
    /*
        ./umask
        ls -hal
     */
    
    exit(0);
}



