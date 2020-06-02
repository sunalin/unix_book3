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


typedef struct 
{
    long type;
    char buf[1024];
}MSGBUF_T;

int main(int argc, char* args[])
{
    /* 消息队列 可用于不相关进程通信 */

    pid_t pid = fork();
    if (pid > 0)
    {
        /* parent process */

        key_t key = ftok("./Makefile", 0);
        /* IPC_CREAT: key队列不存在则创建，已存在则打开返回
           IPC_CREAT|IPC_EXCL: key队列不存在则创建，已存在则出错返回 */
        int msgid = msgget(key, IPC_CREAT|0666);
        printf("pid[%d] parent  key=0x%X  msgid=%d\r\n", getpid(), key, msgid);

        MSGBUF_T msg;
        while (1)
        {
            // 发
            msg.type = 1;
            strcpy(msg.buf, "str from 1");
            if (msgsnd(msgid, &msg, sizeof(msg.buf), IPC_NOWAIT) < 0)
                perror("msgsnd");
            else
                printf("pid[%d] send type=%d  buf=%s\r\n", getpid(), (int)msg.type, msg.buf);

            // 收
            msg.type = 2;   //0 允许收所有类型，先进先出
            if (msgrcv(msgid, &msg, sizeof(msg.buf), msg.type, IPC_NOWAIT) < 0)
                perror("msgrcv");
            else
                printf("pid[%d] recv type=%d  buf=%s\r\n", getpid(), (int)msg.type, msg.buf);

            static int i = 3;
            if (i-- <= 0)
                break;
            sleep(1);
        }
    
        waitpid(pid, 0, 0);
        if (msgctl(msgid, IPC_RMID, NULL) < 0)  /* 删除消息队列，如果不删除会留在系统里 */
            perror("msgctl");

    }
    else if (pid == 0)
    {
        /* child process */

        key_t key = ftok("./Makefile", 0);
        /* IPC_CREAT: key队列不存在则创建，已存在则打开返回
           IPC_CREAT|IPC_EXCL: key队列不存在则创建，已存在则出错返回 */
        int msgid = msgget(key, IPC_CREAT|0666);
        printf("pid[%d] child   key=0x%X  msgid=%d\r\n", getpid(), key, msgid);
    
        MSGBUF_T msg;
        while (1)
        {
            // 发
            msg.type = 2;
            strcpy(msg.buf, "str from 2");
            if (msgsnd(msgid, &msg, sizeof(msg.buf), IPC_NOWAIT) < 0)
                perror("msgsnd");
            else
                printf("pid[%d] send type=%d  buf=%s\r\n", getpid(), (int)msg.type, msg.buf);

            // 收
            msg.type = 1;   //0 允许收所有类型，先进先出
            if (msgrcv(msgid, &msg, sizeof(msg.buf), msg.type, IPC_NOWAIT) < 0)
                perror("msgrcv");
            else
                printf("pid[%d] recv type=%d  buf=%s\r\n", getpid(), (int)msg.type, msg.buf);

            static int i = 3;
            if (i-- <= 0)
                break;
            sleep(1);
        }
    }


    return 0;
}



