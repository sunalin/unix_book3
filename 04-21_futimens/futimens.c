#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


int main(int argc, char* args[])
{
    int fd;
    struct stat statbuf;
    struct timespec times[2];

    fd = open("./foo1", O_WRONLY|O_CREAT|O_TRUNC);
    close(fd);
    
    stat("./foo1", &statbuf);
    fd = open("./foo1", O_RDWR|O_TRUNC);    // �޸��ļ�
    times[0] = statbuf.st_atim;
    times[1] = statbuf.st_mtim;

    /* futimens    �����ļ���(����ʱ�� �޸�ʱ��)*/
    futimens(fd, times);    // reset times
    close(fd);
    
    /*

     */
    
    exit(0);
}



