#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


int main(int argc, char* args[])
{
    int fd = open("./foo1", O_WRONLY|O_CREAT|O_TRUNC);

    /* unlink ��path�ļ������Ӽ���-1��������Ϊ0ʱ���ں�ɾ���ļ����ݣ�
       ����н��̴����ļ������ݲ���ɾ����
       �ر��ļ�ʱ���ں˼����ļ��򿪵Ľ��̸�����Ϊ0ʱ�ż�����Ӽ��� */

    unlink("./foo1");   // foo1������

    close(fd);

    /*

     */
    
    exit(0);
}



