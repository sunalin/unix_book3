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


int main(int argc, char* args[])
{
    pid_t pid;
    struct stat sbuf;
    char* fifo_path = "./temp.fifo";


    if (access(fifo_path, F_OK) == -1       /* 文件不存在 */
        || stat(fifo_path, &sbuf) == -1
        || !S_ISFIFO(sbuf.st_mode))       
    {
        /* mkfifo    创建fifo命名管道文件，可用于不相关进程通信，每个进程通过fifo文件访问

           O_RDONLY: open阻塞到有一个进程以写方式打开fifo
           O_WRONLY: open阻塞到有一个进程以读方式打开fifo
           O_RDONLY|O_NONBLOCK: open正常打开，正常返回fd
           O_WRONLY|O_NONBLOCK: 已经有一个进程以读方式打开fifo则正常返回fd，否则返回-1 */
           
        remove(fifo_path);
        if (mkfifo(fifo_path, 0777) != 0)
            perror("mkfifo");
        else
            printf("mkfifo: %s\r\n", fifo_path);
    }


    pid = fork();
    if (pid > 0)
    {
        /* parent process */
        char dat_buf[100];
        int fd_fifo;

        fd_fifo = open(fifo_path, O_WRONLY);            /* 只写阻塞打开FIFO文件 */
        //fd_fifo = open(fifo_path, O_WRONLY|O_NONBLOCK); /* 只写非阻塞打开FIFO文件 */
        memset(dat_buf, 'a', sizeof(dat_buf));
        dat_buf[sizeof(dat_buf) - 1] = '\0';
        write(fd_fifo, dat_buf, sizeof(dat_buf));   /* 写FIFO */

        waitpid(pid, 0, 0);        
        close(fd_fifo);
    }
    else if (pid == 0)
    {
        /* child process */
        char dat_buf[100];
        int fd_fifo;

        fd_fifo = open(fifo_path, O_RDONLY);            /* 只读阻塞打开FIFO文件 */
        //fd_fifo = open(fifo_path, O_RDONLY|O_NONBLOCK); /* 只读非阻塞打开FIFO文件 */
        read(fd_fifo, dat_buf, sizeof(dat_buf));   /* 读FIFO */
        dat_buf[sizeof(dat_buf) - 1] = '\0';
        printf("%s\r\n", dat_buf);

        close(fd_fifo);
    }

    return 0;
}



