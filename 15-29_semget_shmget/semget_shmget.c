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


/* 二元信号量，占用资源 */
void sem_lock(int semid)
{
    /* 创建信号量之后默认值为0，信号量值总是>=0 */
    struct sembuf ops[2];
    ops[0].sem_num = 0;         /* 要操作的信号量的编号为0 */
    ops[0].sem_op = 0;          /* 当前值为0立即返回,否则等待0值 */
    ops[0].sem_flg = 0;         /* IPC_NOWAIT SEM_UNDO */

    ops[1].sem_num = 0;         /* 要操作的信号量的编号为0 */
    ops[1].sem_op = 1;          /* sem_op>0(当前值+sem_op), sem_op非0时按需指定SEM_UNDO */
    ops[1].sem_flg = SEM_UNDO;  /* IPC_NOWAIT SEM_UNDO */

    if (semop(semid, ops, 2) < 0)
        perror("semop");
}

/* 二元信号量，释放资源 */
void sem_unlock(int semid)
{
    /* 创建信号量之后默认值为0，信号量值总是>=0 */
    struct sembuf ops[1];
    ops[0].sem_num = 0;         /* 要操作的信号量的编号为0 */
    ops[0].sem_op = -1;         /* sem_op<0(当前值够sem_op减则立即返回,否则等待), sem_op非0时按需指定SEM_UNDO */
    ops[0].sem_flg = SEM_UNDO;  /* IPC_NOWAIT SEM_UNDO */

    if (semop(semid, ops, 1) < 0)
        perror("semop");
}

int main(int argc, char* args[])
{
    /* 信号量保护共享内存，可用于不相关进程通信 */

    pid_t pid = fork();
    if (pid > 0)
    {
        /* parent process */


        /* IPC_CREAT: key不存在则创建，已存在则打开返回 IPC_CREAT|IPC_EXCL: key不存在则创建，已存在则出错返回 */
        key_t key = ftok("./Makefile", 0);
        /* 信号量集 */
        int semid = semget(key, 1, IPC_CREAT|0666);/* nsems=0(引用现存的),nsems=个数(创建信号量集中的信号量个数) */
        if (semid < 0)
            perror("semget");
        printf("pid[%d] parent  key=0x%X  semid=%d\r\n", getpid(), key, semid);
        /* 共享内存 */
        int shmid = shmget(key, 200, IPC_CREAT|0666);/* size=0(引用现存的),size=字节数(创建新内存) */
        if (shmid < 0)
            perror("shmget");
        printf("pid[%d] parent  key=0x%X  shmid=%d\r\n", getpid(), key, shmid);


        char* buf = shmat(shmid, 0, SHM_RND);   /* 共享内存链接到本进程 */
        while (1)
        {
            static volatile int i = 5;
            if (--i < 0)
                break;

            sem_lock(semid);    /* 用信号量保护buf */
            buf[0] = '\0';
            strcpy(buf, "buf value is 0");
            buf[strlen(buf)-1] = (i+'0');
            printf("pid[%d] A: %s\r\n", getpid(), buf);
            sem_unlock(semid);

            sleep(2);
        }
        shmdt(buf);  /* 共享内存从本进程分离 */
        
        
        waitpid(pid, 0, 0);
        if (semctl(semid, 0, IPC_RMID, NULL) < 0)   /* 删除信号量集，如果不删除会留在系统里 */
           perror("semctl");
        if (shmctl(shmid, IPC_RMID, NULL) < 0)      /* 删除共享内存，如果不删除会留在系统里 */
           perror("shmctl");
    }
    else if (pid == 0)
    {
        /* child process */
        sleep(2);


        /* IPC_CREAT: key不存在则创建，已存在则打开返回 IPC_CREAT|IPC_EXCL: key不存在则创建，已存在则出错返回 */
        key_t key = ftok("./Makefile", 0);
        /* 信号量集 */
        int semid = semget(key, 0, IPC_CREAT|0666);/* nsems=0(引用现存的),nsems=个数(创建信号量集中的信号量个数) */
        if (semid < 0)
            perror("semget");
        printf("pid[%d] child   key=0x%X  semid=%d\r\n", getpid(), key, semid);
        /* 共享内存 */
        int shmid = shmget(key, 0, IPC_CREAT|0666);/* size=0(引用现存的),size=字节数(创建新内存) */
        if (shmid < 0)
            perror("shmget");
        printf("pid[%d] child   key=0x%X  shmid=%d\r\n", getpid(), key, shmid);


        char* buf = shmat(shmid, 0, SHM_RND);   /* 共享内存链接到本进程 */
        while (1)
        {
            static volatile int i = 5;
            if (--i < 0)
                break;

            sem_lock(semid);    /* 用信号量保护buf */
            printf("pid[%d] B: %s\r\n", getpid(), buf);
            sem_unlock(semid);

            sleep(1);
        }
        shmdt(buf);  /* 共享内存从本进程分离 */
    }

    /* https://blog.csdn.net/qq_31073871/article/details/80937148
       https://blog.csdn.net/qq_31073871/article/details/80922258 */
    
    return 0;
}



