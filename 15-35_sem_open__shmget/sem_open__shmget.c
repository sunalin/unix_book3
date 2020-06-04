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


typedef struct
{
    char        name[_POSIX_NAME_MAX];
    sem_t*      sem;
}sem_name_t;

int sem_lock(sem_t* sem)
{
    /* 等待满足信号量值能够-1: 
       1、如果当前信号量值>0，信号量值-1后立即返回
       2、如果当前信号量值=0，阻塞等待信
       注: 信号量值总是>=0 */
    return sem_wait(sem);
}

int sem_trylock(sem_t* sem)
{
    /* 等待满足信号量值能够-1: 
       1、如果当前信号量值>0，信号量值-1后立即返回
       2、如果当前信号量值=0，阻塞等待信
       注: 信号量值总是>=0 */
    return sem_trywait(sem);    /* 不等待 */
}

int sem_timelock(sem_t* sem, const struct timespec* abstime)
{
    /* 等待满足信号量值能够-1: 
       1、如果当前信号量值>0，信号量值-1后立即返回
       2、如果当前信号量值=0，阻塞等待信
       注: 信号量值总是>=0 */
    return sem_timedwait(sem, abstime); /* 带超时等待 */
}

int sem_unlock(sem_t* sem)
{
    /* 使当前信号量值+1 */
    return sem_post(sem);
}




typedef struct
{
    char*       buf;
    sem_name_t  sem_name;
}MY_DATA;

int main(int argc, char* args[])
{
    /* POSIX命名信号量(类似互斥量)保护共享内存，可用于不相关进程通信 */

    pid_t pid = fork();
    if (pid > 0)
    {
        /* parent process */


        MY_DATA shared_data;
        /* POSIX命名信号量 */
        snprintf(shared_data.sem_name.name,
                 sizeof(shared_data.sem_name.name), "%s", "/sem_name0");
        /* sem_open
           oflag: 0               使用现有命名信号量(忽略mode、value)
                  O_CREAT         使用现有命名信号量(忽略mode、value)，不存在则创建(使用mode、value)
                  O_CREAT|O_EXCL  如果命名信号量已存在sem_open返回出错，不存在则创建(使用mode、value)
           mode: 权限
           value: 0~SEM_VALUE_MAX 信号量初始值 */
        shared_data.sem_name.sem = sem_open(shared_data.sem_name.name,
                                            O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO, 1);
        if (shared_data.sem_name.sem == SEM_FAILED || errno == EEXIST)
            perror("sem_open");

        /* 共享内存 */
        key_t key = ftok("./Makefile", 0);
        /* IPC_CREAT: key不存在则创建，已存在则打开返回 IPC_CREAT|IPC_EXCL: key不存在则创建，已存在则出错返回 */
        int shmid = shmget(key, 200, IPC_CREAT|0666);/* size=0(引用现存的),size=字节数(创建新内存) */
        if (shmid < 0)
            perror("shmget");
        printf("pid[%d] parent  key=0x%X  shmid=%d\r\n", getpid(), key, shmid);


        shared_data.buf = shmat(shmid, 0, SHM_RND);   /* 共享内存链接到本进程 */
        while (1)
        {
            static volatile int i = 5;
            if (--i < 0)
                break;

            sem_lock(shared_data.sem_name.sem);    /* 用信号量保护buf */
            shared_data.buf[0] = '\0';
            strcpy(shared_data.buf, "buf value is 0");
            shared_data.buf[strlen(shared_data.buf)-1] = (i+'0');
            printf("pid[%d] A: %s\r\n", getpid(), shared_data.buf);
            sem_unlock(shared_data.sem_name.sem);

            sleep(2);
        }
        shmdt(shared_data.buf);  /* 共享内存从本进程分离 */

        
        waitpid(pid, 0, 0);
        if (sem_close(shared_data.sem_name.sem) < 0)    /* 释放命名信号量在进程占用的资源, 不会删除信号量 */
            perror("sem_close");
        if (sem_unlink(shared_data.sem_name.name) < 0)  /* 删除命名信号量，如果不删除会留在系统里 */
           perror("sem_unlink");
        if (shmctl(shmid, IPC_RMID, NULL) < 0)          /* 删除共享内存，如果不删除会留在系统里 */
           perror("shmctl");
    }
    else if (pid == 0)
    {
        /* child process */
        sleep(2);


        MY_DATA shared_data;
        /* POSIX命名信号量 */
        snprintf(shared_data.sem_name.name,
                 sizeof(shared_data.sem_name.name), "%s", "/sem_name0");
        /* sem_open
           oflag: 0               使用现有命名信号量(忽略mode、value)
                  O_CREAT         使用现有命名信号量(忽略mode、value)，不存在则创建(使用mode、value)
                  O_CREAT|O_EXCL  如果命名信号量已存在sem_open返回出错，不存在则创建(使用mode、value)
           mode: 权限
           value: 0~SEM_VALUE_MAX 信号量初始值 */
        shared_data.sem_name.sem = sem_open(shared_data.sem_name.name,
                                            0, S_IRWXU|S_IRWXG|S_IRWXO, 1);
        if (shared_data.sem_name.sem == SEM_FAILED || errno == EEXIST)
            perror("sem_open");

        /* 共享内存 */
        key_t key = ftok("./Makefile", 0);
        /* IPC_CREAT: key不存在则创建，已存在则打开返回 IPC_CREAT|IPC_EXCL: key不存在则创建，已存在则出错返回 */
        int shmid = shmget(key, 0, IPC_CREAT|0666);/* size=0(引用现存的),size=字节数(创建新内存) */
        if (shmid < 0)
            perror("shmget");
        printf("pid[%d] child   key=0x%X  shmid=%d\r\n", getpid(), key, shmid);


        shared_data.buf = shmat(shmid, 0, SHM_RND);   /* 共享内存链接到本进程 */
        while (1)
        {
            static volatile int i = 5;
            if (--i < 0)
                break;

            sem_lock(shared_data.sem_name.sem);    /* 用信号量保护buf */
            printf("pid[%d] B: %s\r\n", getpid(), shared_data.buf);
            sem_unlock(shared_data.sem_name.sem);

            sleep(1);
        }
        shmdt(shared_data.buf);  /* 共享内存从本进程分离 */
    }
    
    /* https://www.freesion.com/article/1895162671/ */
    
    return 0;
}



