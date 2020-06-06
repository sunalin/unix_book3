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

    /* connect to server 127.0.0.1:3501 */
#define MAXSLEEP    128
    for (int sec = 1; sec <= MAXSLEEP; sec++)
    {
        sockfd = socket(AF_INET, SOCK_STREAM|SOCK_CLOEXEC/*|SOCK_NONBLOCK*/, 0);
        if (sockfd < 0
            || connect(sockfd, (struct sockaddr*)&addr_in, sizeof(addr_in)) < 0)
        {        
            perror("client socket connect");
            shutdown(sockfd, SHUT_RDWR);
            close(sockfd);
            sockfd = -1;
        }
        else
        {
            /* connect sucess */
            print_local_peer(sockfd, "client connect");
            
            /* SOCK_NONBLOCK 非阻塞(connect成功之后才能设置) */
            //int flags = fcntl(sockfd, F_GETFL, 0);
            //flags |= O_NONBLOCK;
            //fcntl(sockfd, F_SETFL, flags);
        
            break;
        }
        if (sec <= MAXSLEEP/2)
            sleep(sec);
    }


    /* recv server msg */
    int n = 0;
    char recv_buf[128];
    while (1)
    {
        n = recv(sockfd, recv_buf, sizeof(recv_buf), 0);
        if (n < 0)
            perror("client recv");
        if (n > 0 && n < (sizeof(recv_buf) - 1))
        {
            recv_buf[n] = '\0';
            printf("client recv: %s\n", recv_buf);

            fflush(0);
            if (strstr(recv_buf, "quit"))
                break;
        }
        
        sleep(1);
    }
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

    /* server listen 127.0.0.1:3501 */
    sockfd = socket(AF_INET, SOCK_STREAM|SOCK_CLOEXEC/*|SOCK_NONBLOCK*/, 0);
    if (sockfd < 0
        || bind(sockfd, (struct sockaddr*)&addr_in, sizeof(addr_in)) < 0
        || listen(sockfd, 10) < 0) /* 请求连接队列长度10，accept会处理请求 */
    {
        perror("server socket bind listen");
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
        sockfd = -1;
        return;
    }
    else
    {
        /* listen sucess */
        print_local_peer(sockfd, "server listen");

        /* SOCK_NONBLOCK 非阻塞 */
        //int flags = fcntl(sockfd, F_GETFL, 0);
        //flags |= O_NONBLOCK;
        //fcntl(sockfd, F_SETFL, flags);
    }

    /* server accept */
    char send_buf[128];
    while (1)
    {
        int sockfd_client = accept(sockfd, 0, 0);
        if (sockfd_client < 0)
            perror("server accept");
        else
        {        
            print_local_peer(sockfd_client, "server accept");

            /* SOCK_CLOEXEC */
            int flags = fcntl(sockfd_client, F_GETFL, 0);
            flags |= SOCK_CLOEXEC;
            fcntl(sockfd_client, F_SETFL, flags);

            FILE* fp = popen("/usr/bin/uptime", "r");
            while (fgets(send_buf, sizeof(send_buf), fp) != 0)
            {
                printf("server send: %s\n", send_buf);
                send(sockfd_client, send_buf, strlen(send_buf), 0);
                usleep(500);
            }
            pclose(fp);

            
            // quit
            snprintf(send_buf, sizeof(send_buf), "%s", "quit");
            send_buf[strlen("quit")] = '\0';
            printf("server send: %s\n", send_buf);
            send(sockfd_client, send_buf, strlen(send_buf), 0);            
            fflush(0);
            if (strstr(send_buf, "quit"))
                break;
        }
        
        shutdown(sockfd_client, SHUT_RDWR);
        close(sockfd_client);
        sleep(1);
    }
    
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
}

int main(int argc, char* args[])
{
    /* socket AF_INET(ipv4), SOCK_STREAM(TCP)

       应用层：TFTP，HTTP，SNMP，FTP，SMTP，DNS，Telnet 等等
       传输层：TCP，UDP
       网络层：IP，ICMP，OSPF，EIGRP，IGMP
       数据链路层：SLIP，CSLIP，PPP，MTU */

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



