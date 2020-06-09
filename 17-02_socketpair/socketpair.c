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
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <limits.h>
#include <semaphore.h>  // -lpthread

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <poll.h>


int main(int argc, char* args[])
{
    /* unix域套接字(未命名): 全双工管道，用于相关进程通信/线程通信
       AF_UNIX(unix域套接字) SOCK_STREAM(字节流)/SOCK_DGRAM(数据报)

       socketpair    创建unix域套接字(未命名): 全双工管道
       s[0]写/读 <<==>> s[1]写/读
       注: 1、只有对面端写入了数据后本端才能读出数据
           2、例: 父进程使用s[0]写/读, 子进程使用s[1]写/读, 反之亦可
           3、同一进程也可以，只需满足上述第1点要求

       https://blog.csdn.net/weixin_40039738/article/details/81095013 */


    pid_t pid;
    int sockfd[2];

    /* socketpair    创建unix域套接字(未命名): 全双工管道 */
    if (socketpair(AF_UNIX, SOCK_STREAM/*SOCK_DGRAM*/, 0, sockfd) < 0)
        perror("socketpair");
    
    pid = fork();
    if (pid > 0)
    {
        /* parent process */
        char buf[128];
        struct pollfd pfd;
        int fd = sockfd[0]; // 只使用sockfd[0]读/写
        close(sockfd[1]);


        snprintf(buf, sizeof(buf), "%s", "123");
        write(fd, buf, strlen(buf)+1);
        printf("A write: %s\n", buf);

        
        /* 等待可读 */
        pfd.fd = fd;
        pfd.events = POLLIN;    /* fd数据可读 */
        pfd.revents = -1;
        poll(&pfd, sizeof(pfd)/sizeof(struct pollfd), -1);  /* -1阻塞等待(不超时) */
        if (pfd.revents = POLLIN)
        {
            read(pfd.fd, buf, sizeof(buf));
            buf[sizeof(buf)-1] = '\0';
            printf("A read: %s\n", buf);
        }


        close(sockfd[0]);
        waitpid(pid, 0, 0);
    }
    else if (pid == 0)
    {
        /* child process */
        char buf[128];
        struct pollfd pfd;
        int fd = sockfd[1]; // 只使用sockfd[1]读/写
        close(sockfd[0]);


        snprintf(buf, sizeof(buf), "%s", "456");
        write(fd, buf, strlen(buf)+1);
        printf("B write: %s\n", buf);

        
        /* 等待可读 */
        pfd.fd = fd;
        pfd.events = POLLIN;    /* fd数据可读 */
        pfd.revents = -1;
        poll(&pfd, sizeof(pfd)/sizeof(struct pollfd), -1);  /* -1阻塞等待(不超时) */
        if (pfd.revents = POLLIN)
        {
            read(pfd.fd, buf, sizeof(buf));
            buf[sizeof(buf)-1] = '\0';
            printf("B read: %s\n", buf);
        }
    }

    return 0;
}



