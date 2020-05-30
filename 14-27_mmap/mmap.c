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
    int fdin;
    int fdout;
    char* src;
    char* dst;
    struct stat sbuf;


    fdin  = open("./Makefile", O_RDONLY);
    fdout = open("./temp.out", O_RDWR|O_CREAT|O_TRUNC);
    if (fdin < 0 || fdout < 0)
        perror("open");

    fstat(fdin, &sbuf);                     /* ��ȡԭ�ļ����� */
    //lseek(fdout, sbuf.st_size, SEEK_SET);   /* ��������ļ����� */
    ftruncate(fdout, sbuf.st_size);         /* ��������ļ����� */


    /* mmap(aar,len,port,flag,fd,off)    �ɽ������ļ�ӳ�䵽һ���������ϣ�
       �Ի���ִ�ж�д���൱�ڶ��ļ�ִ�ж�д����������read/writeҲ����ִ��IO��д

       addr:ӳ�仺�����׵�ַ��Ϊ0ʱ��ϵͳָ����ͨ������ֵ���أ�ӳ��֮���ͨ���õ�ַ����
       len: ӳ�仺��������
       prot:ָ����д(Ҫ��fd���������Ӧ)
       flag: MAP_SHARED(ֱ�Ӳ����ļ�) MAP_PRIVATE(�����ļ��ĸ���)
       fd:  �ļ�������
       off: �ļ�ƫ����(�ļ��Ӵ�ƫ������ʼӳ��len����) */

    src = mmap(0, sbuf.st_size, PROT_READ,            MAP_SHARED, fdin,  0);
    dst = mmap(0, sbuf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fdout, 0);
    if (!src || !dst)
        perror("mmap");

    memcpy(dst, src, sbuf.st_size); /* �Ի���ִ�ж�д���൱�ڶ��ļ�ִ�ж�д */    
    msync(src, sbuf.st_size, MS_SYNC);   /* ˢ��ӳ�䵽�ļ� */ //fsync(fd)
    msync(dst, sbuf.st_size, MS_SYNC);   /* ˢ��ӳ�䵽�ļ� */ //fsync(fd)
    printf("copy success...\r\n");

    munmap(src, sbuf.st_size);   /* ���ӳ�䣬MAP_PRIVATE���ļ��������� */
    munmap(dst, sbuf.st_size);   /* ���ӳ�䣬MAP_PRIVATE���ļ��������� */
    close(fdin);
    close(fdout);

    /* �����Ǵ�ӡ���� */
    printf("===============================================\r\n");
    system("cat Makefile");
    printf("===============================================\r\n");
    system("cat temp.out");
    system("rm temp.out");

    exit(0);
}



