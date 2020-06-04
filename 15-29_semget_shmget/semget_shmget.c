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


/* ��Ԫ�ź�����ռ����Դ */
void sem_lock(int semid)
{
    /* �����ź���֮��Ĭ��ֵΪ0���ź���ֵ����>=0 */
    struct sembuf ops[2];
    ops[0].sem_num = 0;         /* Ҫ�������ź����ı��Ϊ0 */
    ops[0].sem_op = 0;          /* ��ǰֵΪ0��������,����ȴ�0ֵ */
    ops[0].sem_flg = 0;         /* IPC_NOWAIT SEM_UNDO */

    ops[1].sem_num = 0;         /* Ҫ�������ź����ı��Ϊ0 */
    ops[1].sem_op = 1;          /* sem_op>0(��ǰֵ+sem_op), sem_op��0ʱ����ָ��SEM_UNDO */
    ops[1].sem_flg = SEM_UNDO;  /* IPC_NOWAIT SEM_UNDO */

    if (semop(semid, ops, 2) < 0)
        perror("semop");
}

/* ��Ԫ�ź������ͷ���Դ */
void sem_unlock(int semid)
{
    /* �����ź���֮��Ĭ��ֵΪ0���ź���ֵ����>=0 */
    struct sembuf ops[1];
    ops[0].sem_num = 0;         /* Ҫ�������ź����ı��Ϊ0 */
    ops[0].sem_op = -1;         /* sem_op<0(��ǰֵ��sem_op������������,����ȴ�), sem_op��0ʱ����ָ��SEM_UNDO */
    ops[0].sem_flg = SEM_UNDO;  /* IPC_NOWAIT SEM_UNDO */

    if (semop(semid, ops, 1) < 0)
        perror("semop");
}

int main(int argc, char* args[])
{
    /* �ź������������ڴ棬�����ڲ���ؽ���ͨ�� */

    pid_t pid = fork();
    if (pid > 0)
    {
        /* parent process */


        /* IPC_CREAT: key�������򴴽����Ѵ�����򿪷��� IPC_CREAT|IPC_EXCL: key�������򴴽����Ѵ���������� */
        key_t key = ftok("./Makefile", 0);
        /* �ź����� */
        int semid = semget(key, 1, IPC_CREAT|0666);/* nsems=0(�����ִ��),nsems=����(�����ź������е��ź�������) */
        if (semid < 0)
            perror("semget");
        printf("pid[%d] parent  key=0x%X  semid=%d\r\n", getpid(), key, semid);
        /* �����ڴ� */
        int shmid = shmget(key, 200, IPC_CREAT|0666);/* size=0(�����ִ��),size=�ֽ���(�������ڴ�) */
        if (shmid < 0)
            perror("shmget");
        printf("pid[%d] parent  key=0x%X  shmid=%d\r\n", getpid(), key, shmid);


        char* buf = shmat(shmid, 0, SHM_RND);   /* �����ڴ����ӵ������� */
        while (1)
        {
            static volatile int i = 5;
            if (--i < 0)
                break;

            sem_lock(semid);    /* ���ź�������buf */
            buf[0] = '\0';
            strcpy(buf, "buf value is 0");
            buf[strlen(buf)-1] = (i+'0');
            printf("pid[%d] A: %s\r\n", getpid(), buf);
            sem_unlock(semid);

            sleep(2);
        }
        shmdt(buf);  /* �����ڴ�ӱ����̷��� */
        
        
        waitpid(pid, 0, 0);
        if (semctl(semid, 0, IPC_RMID, NULL) < 0)   /* ɾ���ź������������ɾ��������ϵͳ�� */
           perror("semctl");
        if (shmctl(shmid, IPC_RMID, NULL) < 0)      /* ɾ�������ڴ棬�����ɾ��������ϵͳ�� */
           perror("shmctl");
    }
    else if (pid == 0)
    {
        /* child process */
        sleep(2);


        /* IPC_CREAT: key�������򴴽����Ѵ�����򿪷��� IPC_CREAT|IPC_EXCL: key�������򴴽����Ѵ���������� */
        key_t key = ftok("./Makefile", 0);
        /* �ź����� */
        int semid = semget(key, 0, IPC_CREAT|0666);/* nsems=0(�����ִ��),nsems=����(�����ź������е��ź�������) */
        if (semid < 0)
            perror("semget");
        printf("pid[%d] child   key=0x%X  semid=%d\r\n", getpid(), key, semid);
        /* �����ڴ� */
        int shmid = shmget(key, 0, IPC_CREAT|0666);/* size=0(�����ִ��),size=�ֽ���(�������ڴ�) */
        if (shmid < 0)
            perror("shmget");
        printf("pid[%d] child   key=0x%X  shmid=%d\r\n", getpid(), key, shmid);


        char* buf = shmat(shmid, 0, SHM_RND);   /* �����ڴ����ӵ������� */
        while (1)
        {
            static volatile int i = 5;
            if (--i < 0)
                break;

            sem_lock(semid);    /* ���ź�������buf */
            printf("pid[%d] B: %s\r\n", getpid(), buf);
            sem_unlock(semid);

            sleep(1);
        }
        shmdt(buf);  /* �����ڴ�ӱ����̷��� */
    }

    /* https://blog.csdn.net/qq_31073871/article/details/80937148
       https://blog.csdn.net/qq_31073871/article/details/80922258 */
    
    return 0;
}



