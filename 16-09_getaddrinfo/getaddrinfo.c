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


typedef struct
{
    char*       buf;
    //sem_name_t  sem_name;
}MY_DATA;


int main(int argc, char* args[])
{
    /* 解析域名的ip地址 */

    int err;
    struct addrinfo hints;
    struct addrinfo* info_res;
    struct sockaddr_in* addr;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;


    char* baidu = "www.baidu.com";
    if (err = getaddrinfo(baidu, "123", &hints, &info_res) != 0)
        printf("getaddrinfo [%d]: %s\n", err, gai_strerror(err));
    addr = (struct sockaddr_in*)info_res->ai_addr;
    printf("%s\t ip:[%s]\t port:[%d]\n",
           baidu, inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));


    char* ip_str = "127.0.0.255";
    if (err = getaddrinfo(ip_str, "123", &hints, &info_res) != 0)
        printf("getaddrinfo [%d]: %s\n", err, gai_strerror(err));
    addr = (struct sockaddr_in*)info_res->ai_addr;
    printf("%s\t ip:[%s]\t port:[%d]\n",
           ip_str, inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
    
    
    char* localhost = "localhost";
    if (err = getaddrinfo(localhost, "123", &hints, &info_res) != 0)
        printf("getaddrinfo [%d]: %s\n", err, gai_strerror(err));
    addr = (struct sockaddr_in*)info_res->ai_addr;
    printf("%s\t ip:[%s]\t port:[%d]\n",
           localhost, inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));

    
    freeaddrinfo(info_res);
    
    return 0;
}



