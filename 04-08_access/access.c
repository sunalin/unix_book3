#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


int main(int argc, char* args[])
{
    /* access faccessat 文件访问权限测试(读/写/可执行) */

    if (argc != 2)
    {
        printf("usage: access <pathname>\r\n");
        return 0;
    }

    if (access(args[1], R_OK) < 0)
        printf("[%s]access read ERROR\r\n", args[1]);
    else
        printf("[%s]access read OK\r\n", args[1]);

    if (open(args[1], O_RDONLY) < 0)
        printf("[%s]open read only ERROR\r\n", args[1]);
    else
        printf("[%s]open read only OK\r\n", args[1]);
    
    /*
        ./stat_fstat_lstat /vmlinuz /etc /dev/tty /dev/sda /var/spool/cron/FIFO /bin /dev/printer
     */
    
    return 0;
}



