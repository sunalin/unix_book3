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
#include <sys/un.h>     // struct sockaddr_un


void client(void)
{
    int sockfd;
    struct sockaddr_un addr_un;

    memset(&addr_un, 0, sizeof(addr_un));
    addr_un.sun_family = AF_UNIX;
    strcpy(addr_un.sun_path, "./temp.socket");

    while (1)
    {
        /* socket */
        sockfd = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC/*|SOCK_NONBLOCK*/, 0);

        /* ���ӵ�ָ�������� */
        if (connect(sockfd, (struct sockaddr*)&addr_un, sizeof(addr_un)) < 0)
        {
            perror("client connect");
            close(sockfd);
        }
        else
        {
            perror("client connect");
            break;
        }
        sleep(1);
    }


    /* msg */
    int len = 0;
    char buf[128];
    while (1)
    {
        /* �������� */
        snprintf(buf, sizeof(buf), "%s", "123");
        send(sockfd, buf, strlen(buf)+1, 0);
        printf("client send: %s\n", buf);

        
        /* �������� */
        len = recv(sockfd, buf, sizeof(buf), 0);
        buf[len] = '\0';
        printf("client recv: %s\n", buf);


        // quit
        if (strstr(buf, "quit"))
            break;
        sleep(1);
    }

    fflush(0);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
}

void server(void)
{
    int sockfd;
    struct sockaddr_un addr_un;

    memset(&addr_un, 0, sizeof(addr_un));
    addr_un.sun_family = AF_UNIX;
    strcpy(addr_un.sun_path, "./temp.socket");

    /* socket */
    sockfd = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC/*|SOCK_NONBLOCK*/, 0);

    /* �뱾���ļ����а� */
    bind(sockfd, (struct sockaddr*)&addr_un, sizeof(addr_un));
#if 1   // SOCK_STREAM SOCK_SEQPACKET
    while (listen(sockfd, 10) < 0)  /* ��ʼ���� �������Ӷ��г���10 */
    {
        perror("server listen");
        sleep(1);
    }
    perror("server listen");
#endif
    

    /* msg */
    int len = 0;
    char buf[128];
    while (1)
    {
        /* ����connect���� */
        int sockfd_client = accept(sockfd, 0, 0);
        if (sockfd_client < 0)
        {
            perror("server accept");
            sleep(1);
            continue;
        }
        len = recv(sockfd_client, buf, sizeof(buf), 0);
        buf[len] = '\0';
        printf("server recv: %s\n", buf);
        

        /* �ظ���Ϣ */
        snprintf(buf, sizeof(buf), "%s", "456 quit");
        send(sockfd_client, buf, strlen(buf)+1, 0);
        printf("server send: %s\n", buf);


        // quit
        fflush(0);
        shutdown(sockfd_client, SHUT_RDWR);
        close(sockfd_client);
        break;
    }
    
    fflush(0);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
}

int main(int argc, char* args[])
{
    /* socketͨ�� AF_UNIX(unix���׽���_����), SOCK_STREAM(�ֽ���), IPPROTO_TCP(TCP)

       unix���׽���_����: ȫ˫���ܵ��������ڲ���ؽ���ͨ��,
       �ᴴ��һ������socket�ļ�, �����������̿�ͨ�����ļ����ӵ�������

       Ӧ�ò㣺TFTP��HTTP��SNMP��FTP��SMTP��DNS��Telnet �ȵ�
       ����㣺TCP��UDP
       ����㣺IP��ICMP��OSPF��EIGRP��IGMP
       ������·�㣺SLIP��CSLIP��PPP��MTU */

    pid_t pid = fork();
    if (pid > 0)
    {
        unlink("./temp.socket");    /* ɾ���ļ� */
        server();
        waitpid(pid, 0, 0);
        unlink("./temp.socket");    /* ɾ���ļ� */
    }
    else if (pid == 0)
    {
        client();
    }
    
    return 0;
}



