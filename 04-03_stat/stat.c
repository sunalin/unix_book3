#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


int main(int argc, char* args[])
{
    /* stat fstat lstat ��ӡ�ļ����� */

    struct stat st;
    char* ptr;

    for (int i = 0; i < argc; i++)
    {
        printf("[%s]:", args[i]);
        if (lstat(args[i], &st) < 0)
        {
            printf("lstat error\r\n");
            continue;
        }

        if      (S_ISREG(st.st_mode))   printf("S_ISREG  \r\n");    // ��ͨ�ļ�
        else if (S_ISDIR(st.st_mode))   printf("S_ISDIR  \r\n");    // Ŀ¼�ļ�
        else if (S_ISCHR(st.st_mode))   printf("S_ISCHR  \r\n");    // �ַ��豸�ļ�
        else if (S_ISBLK(st.st_mode))   printf("S_ISBLK  \r\n");    // ���豸�ļ�
        else if (S_ISFIFO(st.st_mode))  printf("S_ISFIFO \r\n");    // �ܵ���FIFO
        else if (S_ISLNK(st.st_mode))   printf("S_ISLNK  \r\n");    // ��������
        else if (S_ISSOCK(st.st_mode))  printf("S_ISSOCK \r\n");    // �׽���
    }
    /*
        ./stat /vmlinuz /etc /dev/tty /dev/sda /var/spool/cron/FIFO /bin /dev/printer
     */
    
    return 0;
}



