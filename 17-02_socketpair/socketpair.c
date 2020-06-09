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
    /* unix���׽���(δ����): ȫ˫���ܵ���������ؽ���ͨ��/�߳�ͨ��
       AF_UNIX(unix���׽���) SOCK_STREAM(�ֽ���)/SOCK_DGRAM(���ݱ�)

       socketpair    ����unix���׽���(δ����): ȫ˫���ܵ�
       s[0]д/�� <<==>> s[1]д/��
       ע: 1��ֻ�ж����д�������ݺ󱾶˲��ܶ�������
           2����: ������ʹ��s[0]д/��, �ӽ���ʹ��s[1]д/��, ��֮���
           3��ͬһ����Ҳ���ԣ�ֻ������������1��Ҫ��

       https://blog.csdn.net/weixin_40039738/article/details/81095013 */


    pid_t pid;
    int sockfd[2];

    /* socketpair    ����unix���׽���(δ����): ȫ˫���ܵ� */
    if (socketpair(AF_UNIX, SOCK_STREAM/*SOCK_DGRAM*/, 0, sockfd) < 0)
        perror("socketpair");
    
    pid = fork();
    if (pid > 0)
    {
        /* parent process */
        char buf[128];
        struct pollfd pfd;
        int fd = sockfd[0]; // ֻʹ��sockfd[0]��/д
        close(sockfd[1]);


        snprintf(buf, sizeof(buf), "%s", "123");
        write(fd, buf, strlen(buf)+1);
        printf("A write: %s\n", buf);

        
        /* �ȴ��ɶ� */
        pfd.fd = fd;
        pfd.events = POLLIN;    /* fd���ݿɶ� */
        pfd.revents = -1;
        poll(&pfd, sizeof(pfd)/sizeof(struct pollfd), -1);  /* -1�����ȴ�(����ʱ) */
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
        int fd = sockfd[1]; // ֻʹ��sockfd[1]��/д
        close(sockfd[0]);


        snprintf(buf, sizeof(buf), "%s", "456");
        write(fd, buf, strlen(buf)+1);
        printf("B write: %s\n", buf);

        
        /* �ȴ��ɶ� */
        pfd.fd = fd;
        pfd.events = POLLIN;    /* fd���ݿɶ� */
        pfd.revents = -1;
        poll(&pfd, sizeof(pfd)/sizeof(struct pollfd), -1);  /* -1�����ȴ�(����ʱ) */
        if (pfd.revents = POLLIN)
        {
            read(pfd.fd, buf, sizeof(buf));
            buf[sizeof(buf)-1] = '\0';
            printf("B read: %s\n", buf);
        }
    }

    return 0;
}



