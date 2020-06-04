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
    /* �ȴ������ź���ֵ�ܹ�-1: 
       1�������ǰ�ź���ֵ>0���ź���ֵ-1����������
       2�������ǰ�ź���ֵ=0�������ȴ���
       ע: �ź���ֵ����>=0 */
    return sem_wait(sem);
}

int sem_trylock(sem_t* sem)
{
    /* �ȴ������ź���ֵ�ܹ�-1: 
       1�������ǰ�ź���ֵ>0���ź���ֵ-1����������
       2�������ǰ�ź���ֵ=0�������ȴ���
       ע: �ź���ֵ����>=0 */
    return sem_trywait(sem);    /* ���ȴ� */
}

int sem_timelock(sem_t* sem, const struct timespec* abstime)
{
    /* �ȴ������ź���ֵ�ܹ�-1: 
       1�������ǰ�ź���ֵ>0���ź���ֵ-1����������
       2�������ǰ�ź���ֵ=0�������ȴ���
       ע: �ź���ֵ����>=0 */
    return sem_timedwait(sem, abstime); /* ����ʱ�ȴ� */
}

int sem_unlock(sem_t* sem)
{
    /* ʹ��ǰ�ź���ֵ+1 */
    return sem_post(sem);
}




typedef struct
{
    char*       buf;
    sem_name_t  sem_name;
}MY_DATA;

int main(int argc, char* args[])
{
    /* POSIX�����ź���(���ƻ�����)���������ڴ棬�����ڲ���ؽ���ͨ�� */

    pid_t pid = fork();
    if (pid > 0)
    {
        /* parent process */


        MY_DATA shared_data;
        /* POSIX�����ź��� */
        snprintf(shared_data.sem_name.name,
                 sizeof(shared_data.sem_name.name), "%s", "/sem_name0");
        /* sem_open
           oflag: 0               ʹ�����������ź���(����mode��value)
                  O_CREAT         ʹ�����������ź���(����mode��value)���������򴴽�(ʹ��mode��value)
                  O_CREAT|O_EXCL  ��������ź����Ѵ���sem_open���س����������򴴽�(ʹ��mode��value)
           mode: Ȩ��
           value: 0~SEM_VALUE_MAX �ź�����ʼֵ */
        shared_data.sem_name.sem = sem_open(shared_data.sem_name.name,
                                            O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO, 1);
        if (shared_data.sem_name.sem == SEM_FAILED || errno == EEXIST)
            perror("sem_open");

        /* �����ڴ� */
        key_t key = ftok("./Makefile", 0);
        /* IPC_CREAT: key�������򴴽����Ѵ�����򿪷��� IPC_CREAT|IPC_EXCL: key�������򴴽����Ѵ���������� */
        int shmid = shmget(key, 200, IPC_CREAT|0666);/* size=0(�����ִ��),size=�ֽ���(�������ڴ�) */
        if (shmid < 0)
            perror("shmget");
        printf("pid[%d] parent  key=0x%X  shmid=%d\r\n", getpid(), key, shmid);


        shared_data.buf = shmat(shmid, 0, SHM_RND);   /* �����ڴ����ӵ������� */
        while (1)
        {
            static volatile int i = 5;
            if (--i < 0)
                break;

            sem_lock(shared_data.sem_name.sem);    /* ���ź�������buf */
            shared_data.buf[0] = '\0';
            strcpy(shared_data.buf, "buf value is 0");
            shared_data.buf[strlen(shared_data.buf)-1] = (i+'0');
            printf("pid[%d] A: %s\r\n", getpid(), shared_data.buf);
            sem_unlock(shared_data.sem_name.sem);

            sleep(2);
        }
        shmdt(shared_data.buf);  /* �����ڴ�ӱ����̷��� */

        
        waitpid(pid, 0, 0);
        if (sem_close(shared_data.sem_name.sem) < 0)    /* �ͷ������ź����ڽ���ռ�õ���Դ, ����ɾ���ź��� */
            perror("sem_close");
        if (sem_unlink(shared_data.sem_name.name) < 0)  /* ɾ�������ź����������ɾ��������ϵͳ�� */
           perror("sem_unlink");
        if (shmctl(shmid, IPC_RMID, NULL) < 0)          /* ɾ�������ڴ棬�����ɾ��������ϵͳ�� */
           perror("shmctl");
    }
    else if (pid == 0)
    {
        /* child process */
        sleep(2);


        MY_DATA shared_data;
        /* POSIX�����ź��� */
        snprintf(shared_data.sem_name.name,
                 sizeof(shared_data.sem_name.name), "%s", "/sem_name0");
        /* sem_open
           oflag: 0               ʹ�����������ź���(����mode��value)
                  O_CREAT         ʹ�����������ź���(����mode��value)���������򴴽�(ʹ��mode��value)
                  O_CREAT|O_EXCL  ��������ź����Ѵ���sem_open���س����������򴴽�(ʹ��mode��value)
           mode: Ȩ��
           value: 0~SEM_VALUE_MAX �ź�����ʼֵ */
        shared_data.sem_name.sem = sem_open(shared_data.sem_name.name,
                                            0, S_IRWXU|S_IRWXG|S_IRWXO, 1);
        if (shared_data.sem_name.sem == SEM_FAILED || errno == EEXIST)
            perror("sem_open");

        /* �����ڴ� */
        key_t key = ftok("./Makefile", 0);
        /* IPC_CREAT: key�������򴴽����Ѵ�����򿪷��� IPC_CREAT|IPC_EXCL: key�������򴴽����Ѵ���������� */
        int shmid = shmget(key, 0, IPC_CREAT|0666);/* size=0(�����ִ��),size=�ֽ���(�������ڴ�) */
        if (shmid < 0)
            perror("shmget");
        printf("pid[%d] child   key=0x%X  shmid=%d\r\n", getpid(), key, shmid);


        shared_data.buf = shmat(shmid, 0, SHM_RND);   /* �����ڴ����ӵ������� */
        while (1)
        {
            static volatile int i = 5;
            if (--i < 0)
                break;

            sem_lock(shared_data.sem_name.sem);    /* ���ź�������buf */
            printf("pid[%d] B: %s\r\n", getpid(), shared_data.buf);
            sem_unlock(shared_data.sem_name.sem);

            sleep(1);
        }
        shmdt(shared_data.buf);  /* �����ڴ�ӱ����̷��� */
    }
    
    /* https://www.freesion.com/article/1895162671/ */
    
    return 0;
}



