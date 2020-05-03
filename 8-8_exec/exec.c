#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <wait.h>


char* env_paths[] = {"USER=sunalin", "PATH=/tmp", NULL};

int main(int argc, char* args[])
{
    pid_t pid;

    /*=====================================*/
    pid = fork();
    if (pid < 0)
        printf("fork error\r\n");
    else if (pid > 0)
    {
        // parent process
        if (waitpid(pid, 0, 0) < 0)
            printf("waitpid error\r\n");
    }
    else if (pid == 0)
    {
        // child process
        // 7-2_environ: ��ӡ���������в��� ��ӡ���л�������
        if (execle("./environ",
            "environ", "my_arg1", "my Arg2", "aa3", (char*)0,
            env_paths) < 0)
            printf("execle error\r\n");
    }

    /*=====================================*/
    pid = fork();
    if (pid < 0)
        printf("fork error\r\n");
    else if (pid > 0)
    {
        // parent process
        if (waitpid(pid, 0, 0) < 0)
            printf("waitpid error\r\n");
    }
    else if (pid == 0)
    {
        // child process
        // �ӻ����������ļ���Ϊls�ĳ���
        if (execlp("ls", "ls", "-hal", (char*)0) < 0)
            printf("execlp error\r\n");
    }

    /*=====================================*/
    pid = fork();
    if (pid < 0)
        printf("fork error\r\n");
    else if (pid > 0)
    {
        // parent process
        if (waitpid(pid, 0, 0) < 0)
            printf("waitpid error\r\n");
    }
    else if (pid == 0)
    {
        // child process
        // ִ�н������ļ�(������������´������ļ�,�ļ�Ҫ�п�ִ������ chmod 777 sunalin.sh)
        // 
        // execl����������в��� �ᱻ׷���� �������ļ��еĿ�ѡ��������
        // ע:execl��pathname������һ�������в���(�����м�"abc"���滻��"./sunalin.sh")
        if (execl("./sunalin.sh", "abc", "./", (char*)0) < 0)
            printf("execl error\r\n");
    }

    exit(0);
}



