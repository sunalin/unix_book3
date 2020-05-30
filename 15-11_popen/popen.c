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

int main(int argc, char* args[])
{
    char line[8192];
    FILE* fpin;
    FILE* fpout;


    fpin = fopen("./popen.c", "r");
    if (!fpin)
        perror("fopen1");
    

    /* popen(cmd,type)    �����ܵ�������forkһ���ӽ��̣����ӽ���
       �йر�δʹ�õĹܵ��˲�ִ��exec(cmd)�����عܵ�һ�˵�IO�ļ�ָ��

       "r": ���صĹܵ��ļ�ָ������[��]���ܵ���һ�������ӽ��̵�[��׼���]
       "w": ���صĹܵ��ļ�ָ������[д]���ܵ���һ�������ӽ��̵�[��׼����] */
       
    fpout = popen("${PAGER:-more}", "w");   /* �ӻ�����������PAGER,û�ҵ���ʹ��Ĭ��ֵmore */
    if (!fpout)
        perror("fopen2");

    while (fgets(line, sizeof(line), fpin) != NULL)
        fputs(line, fpout); /* д�ܵ� */
    if (ferror(fpin))
        perror("ferror");

    pclose(fpout);      /* �ر��ļ�ָ�� */ 
    fclose(fpin);

    return 0;
}



