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


void set_fl(int fd, int flags)
{
    int val;

    val = fcntl(fd, F_GETFL, 0);
    if (val < 0)
        printf("fcntl F_GETFL error\r\n");

    // turn on flags
    val |= flags;

    if (fcntl(fd, F_SETFL, val) < 0)
        printf("fcntl F_SETFL error\r\n");
}

void clr_fl(int fd, int flags)
{
    int val;

    val = fcntl(fd, F_GETFL, 0);
    if (val < 0)
        printf("fcntl F_GETFL error\r\n");

    // turn flags off
    val &= ~flags;

    if (fcntl(fd, F_SETFL, val) < 0)
        printf("fcntl F_SETFL error\r\n");
}


char buf[100000];

int main(int argc, char* args[])
{
    /* O_NONBLOCK 非阻塞IO，可通过open或fcntl设置此属性 */
    int read_size;
    int write_size;

    read_size = read(STDIN_FILENO, buf, sizeof(buf));
    fprintf(stderr, "read size = %d\r\n", read_size);

    /* 标准输出设为非阻塞 */
    set_fl(STDOUT_FILENO, O_NONBLOCK);
    char* ptr = buf;
    while (read_size > 0)
    {
        errno = 0;
        write_size = write(STDOUT_FILENO, ptr, read_size);
        fprintf(stderr, "write size = %d, errno = %d\r\n",
                write_size, errno);
        if (write_size > 0)
        {
            read_size -= write_size;
            ptr += write_size;
        }
    }
    clr_fl(STDOUT_FILENO, O_NONBLOCK);

    /*
       ./open__O_NONBLOCK < open__O_NONBLOCK.c
       ./open__O_NONBLOCK < open__O_NONBLOCK.c 2>temp.out
     */

    exit(0);
}



