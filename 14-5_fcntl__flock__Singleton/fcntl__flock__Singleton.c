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


int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
    // �������ļ� l_start=0,l_whence=SEEK_SET,l_len=0
    struct flock lock;
    lock.l_type = type;     /* F_RDLCK, F_WRLCK, F_UNLCK */
    lock.l_start = offset;  /* byte offset,relative to l_whence */
    lock.l_whence = whence; /* SEEK_SET, SEEK_CUR, SEEK_END */
    lock.l_len = len;       /* #bytes (0 means to EOF) */

    return fcntl(fd, cmd, &lock);
}

pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len)
{
    // �������ļ� l_start=0,l_whence=SEEK_SET,l_len=0
    struct flock lock;
    lock.l_type = type;     /* F_RDLCK, F_WRLCK, F_UNLCK */
    lock.l_start = offset;  /* byte offset,relative to l_whence */
    lock.l_whence = whence; /* SEEK_SET, SEEK_CUR, SEEK_END */
    lock.l_len = len;       /* #bytes (0 means to EOF) */

    if (fcntl(fd, F_GETLK, &lock) < 0)
        printf("fcntl lock error\r\n");

    if (lock.l_type == F_UNLCK)
        return 0;   // no lock
    else
        return lock.l_pid;  // lock pid
}

/* ���ļ�������� */
#define read_lock(fd,offset,whence,len)     lock_reg(fd,F_SETLK,F_RDLCK,offset,whence,len)
/* ���ļ��������(����) */
#define readw_lock(fd,offset,whence,len)    lock_reg(fd,F_SETLKW,F_RDLCK,offset,whence,len)
/* д�ļ�������� */
#define write_lock(fd,offset,whence,len)    lock_reg(fd,F_SETLK,F_WRLCK,offset,whence,len)
/* д�ļ��������(����) */
#define writew_lock(fd,offset,whence,len)   lock_reg(fd,F_SETLKW,F_WRLCK,offset,whence,len)
/* �ļ�������� */
#define un_lock(fd,offset,whence,len)       lock_reg(fd,F_SETLK,F_UNLCK,offset,whence,len)
/* ���Զ��ļ������� */
#define is_readlock(fd,offset,whence,len)   lock_test(fd,F_RDLCK,offset,whence,len)
/* ����д�ļ������� */
#define is_writelock(fd,offset,whence,len)  lock_test(fd,F_WRLCK,offset,whence,len)

#define FILE_MODE   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define DIR_MODE    (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)


int main(int argc, char* args[])
{
    char PIDFILE[] = "daemon.pid";

    int fd;
    int flags;
    char buf[10];

    fd = open(PIDFILE, O_WRONLY|O_CREAT, FILE_MODE);
    if (fd < 0)
        printf("open error\r\n");

    // try write lock file
    if (write_lock(fd, 0, SEEK_SET, 0) < 0)
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            printf("daemon is already running...\r\n");
            exit(0);
        }
        else
            printf("write_lock error\r\n");
    }

    ftruncate(fd, 0);   // ���Ƚ�Ϊ0
    /* write pid */
    sprintf(buf, "%d\n", getpid());
    if (write(fd, buf, strlen(buf)) != strlen(buf))
        printf("write error\r\n");

    /* ��fork�ӽ��̺��Կ���fd,��exec��ϵͳ�ͻ�ر��ӽ����е�fd */
    flags = fcntl(fd, F_GETFD);  
    flags |= FD_CLOEXEC;  
    fcntl(fd, F_SETFD, flags);

    // do something...
#if 1
    while (1)
        pause();
#endif

    exit(0);
}



