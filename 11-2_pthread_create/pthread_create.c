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
#include <pthread.h>

void* new_thread(void* arg)
{
    printf("%s"
           "pid = %lu    "
           "tid = %lu    0x%lx\r\n",
           "[new thread]",
           (unsigned long)getpid(),
           (unsigned long)pthread_self(), (unsigned long)pthread_self());

    pthread_exit(0);
}

int main(int argc, char* args[])
{
    pthread_t tid;

    if (pthread_create(&tid, 0, new_thread, 0) != 0)
        printf("pthread_create error\r\n");

    printf("%s"
           "pid = %lu    "
           "tid = %lu    0x%lx\r\n",
           "[main thread]",
           (unsigned long)getpid(),
           (unsigned long)pthread_self(), (unsigned long)pthread_self());
    sleep(1);
           
    exit(0);
}



