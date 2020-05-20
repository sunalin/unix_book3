#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <wait.h>


int main(int argc, char* args[])
{
    char* env_paths[] = {"USER=sunalin", "PATH=/tmp", NULL};
    pid_t pid;


    printf("=======================================\r\n");
    pid = fork();
    if (pid > 0)
    {
        // parent process
        waitpid(pid, 0, 0);
    }
    else if (pid == 0)
    {
        // child process
        // �ӻ����������ļ���Ϊls�ĳ���
        if (execlp("ls", "ls", "-a", (char*)0) < 0)
            printf("execlp error\r\n");
    }

    printf("=======================================\r\n");
    pid = fork();
    if (pid > 0)
    {
        // parent process
        waitpid(pid, 0, 0);
    }
    else if (pid == 0)
    {
        // child process
        // environ.c: ��ӡ���������в��� ��ӡ���л�������
        if (execle("./environ", "environ", "argc1", (char*)0, env_paths) < 0)
            printf("execle error\r\n");
    }

    printf("=======================================\r\n");
    pid = fork();
    if (pid > 0)
    {
        // parent process
        waitpid(pid, 0, 0);
    }
    else if (pid == 0)
    {
        // child process
        /* ִ�н������ļ�(������������´������ļ�,�ļ�Ҫ�п�ִ������ chmod 777 environ.sh)
           ��ע��UNIX�Ļ��з���\n,ʹ��git��ȡ��ԭ�ű��ļ�ʱ��ע��

           exec����������в��� �ᱻ׷���� �������ļ��еĿ�ѡ��������
           ע:exec��pathname������һ�������в���(�����м�"argc0"���滻��"./environ.sh") */
        if (execle("./environ.sh", "argc0", "argc1", (char*)0, env_paths) < 0)
            printf("execle error\r\n");
    }

    printf("=======================================\r\n");
    pid = fork();
    if (pid > 0)
    {
        // parent process
        waitpid(pid, 0, 0);
    }
    else if (pid == 0)
    {
        // child process
        if (execl("./bash.sh", "argc0", "argc1", (char*)0) < 0)
            printf("execl error\r\n");
    }
    
    printf("=======================================\r\n");
    pid = fork();
    if (pid > 0)
    {
        // parent process
        waitpid(pid, 0, 0);
    }
    else if (pid == 0)
    {
        // child process
        if (execl("./awk.sh", "argc0", "argc1", (char*)0) < 0)
            printf("execl error\r\n");
    }


    /*
       ./exec
     */

    exit(0);
}



