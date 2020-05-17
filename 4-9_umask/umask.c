#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


int main(int argc, char* args[])
{
#define EXEC_MASK   S_IRWXU|\
                    S_IRWXG|\
                    S_IRWXO

#define MASK1       S_IRUSR|S_IWUSR|\
                    S_IRGRP|S_IWGRP|\
                    S_IROTH|S_IWOTH

#define MASK2       S_IRUSR|S_IWUSR|\
                    S_IRGRP|S_IWGRP
    /*
       umask �������� �ļ�����������(��/д/��ִ�� Ȩ��)
       �����ļ���Ŀ¼�����õ�������
     */
    umask(0);   // ������
    open("./foo1", O_WRONLY|O_CREAT|O_TRUNC, EXEC_MASK);
    umask(MASK1);
    open("./foo2", O_WRONLY|O_CREAT|O_TRUNC, EXEC_MASK);
    umask(MASK2);
    open("./foo3", O_WRONLY|O_CREAT|O_TRUNC, EXEC_MASK);
    
    /*
        ./umask
        ls -hal
     */
    
    exit(0);
}



