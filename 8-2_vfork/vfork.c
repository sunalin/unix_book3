#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/resource.h>


int glob = 5;
char buf[] = "a write to stdout\r\n";

int main(int argc, char* args[])
{
    int var = 10;
    pid_t pid;

    write(STDOUT_FILENO, buf, sizeof(buf) - 1);

    printf("vfork befor[getpid = %d]\r\n", getpid());
    fflush(stdout);
    pid = vfork();
    printf("vfork after[getpid = %d]\r\n", getpid());

    if (pid < 0)
        printf("fork error\r\n");
    else if (pid > 0)
    {
        // parent process
        //sleep(2);
    }
    else if (pid == 0)
    {
        /* 1.vfork()�����ӽ��̣��ڵ���exec()֮ǰ��exit()֮ǰ��
             �ӽ����븸���̹������ݶΣ���fork()��ͬ��forkҪ��
             �������̵����ݶ�,��ջ�Σ�
           2.����vfork()���ӽ�����ִ�У������̱�����ֱ��
             �ӽ��̵�����exec��exit֮�󣬸����̲�ִ�С�
           ʹ��vfork(),���ӽ��̲�����exit()��exec()��������һֱ���𣬵���������*/
        
        // child process
        glob++; // �ӽ����޸ı���,��Ӱ�쵽������,��exec/exit/_exit֮ǰ���ڸ����̵ĵ�ַ�ռ�����,��ʱ�������ѱ�����
        var++;
        exit(0);    // exec/exit/_exit
    }

    printf(" getpid = %d,"
           " getppid = %d,"
           " glob = %d,"
           " var = %d\r\n", getpid(), getppid(), glob, var);

    /*
     ./vfork
     ./vfork > temp.out;cat temp.out
     */

    exit(0);
}




