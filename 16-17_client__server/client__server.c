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


void print_local_peer(int sockfd, char* str)
{
    struct sockaddr_in local;  
    struct sockaddr_in peer;  
    char local_ip[INET_ADDRSTRLEN];
    char peer_ip[INET_ADDRSTRLEN];  
    socklen_t local_len = sizeof(local);  
    socklen_t peer_len  = sizeof(peer);  
    getsockname(sockfd, (struct sockaddr*)&local, &local_len);  
    getpeername(sockfd, (struct sockaddr*)&peer,  &peer_len);  
    inet_ntop(AF_INET, &local.sin_addr, local_ip, sizeof(local_ip));  
    inet_ntop(AF_INET, &peer.sin_addr,  peer_ip,  sizeof(peer_ip));  
    printf("%s[local %s:%d <<==>> peer %s:%d]\n",
           str,
           local_ip, ntohs(local.sin_port),
           peer_ip,  ntohs(peer.sin_port));
}

void client(void)
{
    int sockfd;
    struct sockaddr_in addr_in;

    memset(&addr_in, 0, sizeof(addr_in));
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(3501);
    inet_pton(AF_INET, "127.0.0.1", &addr_in.sin_addr);

    while (1)
    {
        /* socket */
        sockfd = socket(AF_INET, SOCK_STREAM|SOCK_CLOEXEC/*|SOCK_NONBLOCK*/, 0);
        print_local_peer(sockfd, "client socket");
        /* TCP:connect�ɹ���ſ���O_NONBLOCK(������) */
        //int flags = fcntl(sockfd, F_GETFL, 0) | (SOCK_CLOEXEC|O_NONBLOCK);
        //fcntl(sockfd, F_SETFL, flags);

        /* ���ӵ�ָ�������� 127.0.0.1:3501 */
        if (connect(sockfd, (struct sockaddr*)&addr_in, sizeof(addr_in)) < 0)
        {
            perror("client connect");            
            close(sockfd);
        }
        else
        {
            print_local_peer(sockfd, "client connect");
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
        snprintf(buf, sizeof(buf), "%s", "ab");
        send(sockfd, buf, strlen(buf), 0);
        printf("client send: %s\n", buf);

        
        /* �������� */
        len = recv(sockfd, buf, sizeof(buf), 0);
        if (len <= 0)
        {
            perror("client recv");
            sleep(1);
            continue;
        }
        buf[len] = '\0';
        printf("client recv: %s\n", buf);


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
    struct sockaddr_in addr_in;

    memset(&addr_in, 0, sizeof(addr_in));
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(3501);
    inet_pton(AF_INET, "127.0.0.1", &addr_in.sin_addr);

    /* socket */
    sockfd = socket(AF_INET, SOCK_STREAM|SOCK_CLOEXEC/*|SOCK_NONBLOCK*/, 0);
    print_local_peer(sockfd, "server socket");
    /* TCP:connect�ɹ���ſ���O_NONBLOCK(������) */
    //int flags = fcntl(sockfd, F_GETFL, 0) | (SOCK_CLOEXEC|O_NONBLOCK);
    //fcntl(sockfd, F_SETFL, flags);

    bind(sockfd, (struct sockaddr*)&addr_in, sizeof(addr_in));
#if 1   // SOCK_STREAM SOCK_SEQPACKET
    while (listen(sockfd, 10) < 0)  /* ��ʼ���� �������Ӷ��г���10 */
    {
        perror("server listen");
        sleep(1);
    }
    print_local_peer(sockfd, "server listen");
#endif
    

    /* msg */
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
        print_local_peer(sockfd_client, "server accept");
        /* TCP:connect�ɹ���ſ���O_NONBLOCK(������) */
        int flags = fcntl(sockfd_client, F_GETFL, 0) | (SOCK_CLOEXEC/*|O_NONBLOCK*/);
        fcntl(sockfd_client, F_SETFL, flags);


        /* �ظ���Ϣ */
        FILE* fp = popen("/usr/bin/uptime", "r");
        if (fgets(buf, sizeof(buf), fp) != 0)
        {
            send(sockfd_client, buf, strlen(buf), 0);
            printf("server send: %s\n", buf);
        }
        pclose(fp);

        
        // quit
        snprintf(buf, sizeof(buf), "%s", "quit");
        send(sockfd_client, buf, strlen(buf), 0);            
        printf("server send: %s\n", buf);
        if (strstr(buf, "quit"))
            break;
        fflush(0);
        shutdown(sockfd_client, SHUT_RDWR);
        close(sockfd_client);
        sleep(1);
    }
    
    fflush(0);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
}

int main(int argc, char* args[])
{
    /* socketͨ�� AF_INET(ipv4), SOCK_STREAM, IPPROTO_TCP(TCP)

       Ӧ�ò㣺TFTP��HTTP��SNMP��FTP��SMTP��DNS��Telnet �ȵ�
       ����㣺TCP��UDP
       ����㣺IP��ICMP��OSPF��EIGRP��IGMP
       ������·�㣺SLIP��CSLIP��PPP��MTU */

    pid_t pid = fork();
    if (pid > 0)
    {
        server();
        waitpid(pid, 0, 0);
    }
    else if (pid == 0)
    {
        client();
    }
    
    return 0;
}



