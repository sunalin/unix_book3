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

    /* socket */
    sockfd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC/*|SOCK_NONBLOCK*/, 0);
    print_local_peer(sockfd, "client socket");
    /* TCP:connect成功后才可设O_NONBLOCK(非阻塞) */
    //int flags = fcntl(sockfd, F_GETFL, 0) | (SOCK_CLOEXEC|O_NONBLOCK);
    //fcntl(sockfd, F_SETFL, flags);


    /* msg */
    int len = 0;
    char buf[128];
    while (1)
    {
        /* 向指定的服务器发送请求(UDP是无连接的) */
        snprintf(buf, sizeof(buf), "%s", "ab");
        sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&addr_in, sizeof(addr_in));
        printf("client sendto: %s\n", buf);

        
        /* 阻塞接收(我们这里不关心发送者的信息) */
        len = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
        if (len <= 0)
        {
            perror("client recvfrom");
            sleep(1);
            continue;
        }
        buf[len] = '\0';
        printf("client recvfrom: %s\n", buf);


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
    sockfd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC/*|SOCK_NONBLOCK*/, 0);
    print_local_peer(sockfd, "server socket");
    /* TCP:connect成功后才可设O_NONBLOCK(非阻塞) */
    //int flags = fcntl(sockfd, F_GETFL, 0) | (SOCK_CLOEXEC|O_NONBLOCK);
    //fcntl(sockfd, F_SETFL, flags);

    /* SO_REUSEADDR 允许多个(相同/不同)的地址绑定到同一个端口 */
    int val = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    bind(sockfd, (struct sockaddr*)&addr_in, sizeof(addr_in));
#if 0   // SOCK_STREAM SOCK_SEQPACKET
    while (listen(sockfd, 10) < 0)  /* 开始监听 请求连接队列长度10 */
    {
        perror("server listen");
        sleep(1);
    }
    print_local_peer(sockfd, "server listen");
#endif
    

    /* msg */
    int len = 0;
    char buf[128];
    char addr_buf[256]; // client信息
    struct sockaddr_in* addr = (struct sockaddr_in*)addr_buf;
    while (1)
    {
        /* 阻塞接收(获取UDP发送者的连接信息) */
        socklen_t addr_len = sizeof(addr_buf);
        len = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)addr, &addr_len);
        if (len <= 0)
        {
            perror("server recvfrom");
            sleep(1);
            continue;
        }
        buf[len] = '\0';
        printf("server recvfrom: %s\n", buf);


        /* 回复信息给请求者 */
        FILE* fp = popen("/usr/bin/uptime", "r");
        if (fgets(buf, sizeof(buf), fp) != 0)
        {
            sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)addr, addr_len);
            printf("server sendto: %s\n", buf);
        }
        pclose(fp);

        
        // quit
        snprintf(buf, sizeof(buf), "%s", "quit");
        sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)addr, addr_len);
        printf("server sendto: %s\n", buf);
        if (strstr(buf, "quit"))
            break;
        sleep(1);
    }
    
    fflush(0);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
}

int main(int argc, char* args[])
{
    /* socket通信 AF_INET(ipv4), SOCK_DGRAM(数据报), IPPROTO_UDP(UDP)

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
        sleep(2);   /* 让服务器先启动 */
        client();
    }
    
    return 0;
}



