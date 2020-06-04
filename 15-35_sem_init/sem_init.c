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
#include <pthread.h>    // -lpthread


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
    int         value;
    sem_t       sem;
}MY_DATA;
MY_DATA shared_data = {.value = 0,};


void* thread_1(void* arg)
{
    pthread_detach(pthread_self());
    printf("thread_1 run...\r\n");

    int val;
    while (1)
    {
        sem_lock(&shared_data.sem);    /* ���ź���������Դ */
        val = shared_data.value;
        sem_unlock(&shared_data.sem);

        printf("threadid[%lx] B: val[%d]\r\n", pthread_self(), val);
        if (val >= 4)
            break;
        sleep(1);
    }
    
    return 0;
}

int main(int argc, char* args[])
{
    /* POSIXδ�����ź����������̵߳ĵ�ͬ��/���� */


    /* POSIXδ�����ź���(Ҳ����Ϊ�ڴ��ź���)
       pshared: ͨ��Ϊ0
       value:   0~SEM_VALUE_MAX �ź�����ʼֵ*/
    if (sem_init(&shared_data.sem, 0, 1) < 0)   /* ��ʼ��δ�����ź��� */
        perror("sem_init");


    pthread_t tid1;
    pthread_create(&tid1, 0, thread_1, (void*)0);

    int val;
    while (1)
    {    
        sem_lock(&shared_data.sem);    /* ���ź���������Դ */
        val = ++shared_data.value;
        sem_unlock(&shared_data.sem);
    
        printf("threadid[%lx] A: val[%d]\r\n", pthread_self(), val);
        if (val >= 7)
            break;
        sleep(1);
    }

    if (sem_destroy(&shared_data.sem) < 0)      /* �ͷ�δ�����ź��� */
        perror("sem_destroy");

    /* �ź���ͬ��/����
       https://blog.csdn.net/tennysonsky/article/details/46496201 */
    
    return 0;
}



