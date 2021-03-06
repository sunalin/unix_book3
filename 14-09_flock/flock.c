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
    // 锁整个文件 l_start=0,l_whence=SEEK_SET,l_len=0
    struct flock lock;
    lock.l_type = type;     /* F_RDLCK, F_WRLCK, F_UNLCK */
    lock.l_start = offset;  /* byte offset,relative to l_whence */
    lock.l_whence = whence; /* SEEK_SET, SEEK_CUR, SEEK_END */
    lock.l_len = len;       /* #bytes (0 means to EOF) */

    return fcntl(fd, cmd, &lock);
}

pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len)
{
    // 锁整个文件 l_start=0,l_whence=SEEK_SET,l_len=0
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

/* 读文件区域加锁 */
#define read_lock(fd,offset,whence,len)     lock_reg(fd,F_SETLK,F_RDLCK,offset,whence,len)
/* 读文件区域加锁(阻塞) */
#define readw_lock(fd,offset,whence,len)    lock_reg(fd,F_SETLKW,F_RDLCK,offset,whence,len)
/* 写文件区域加锁 */
#define write_lock(fd,offset,whence,len)    lock_reg(fd,F_SETLK,F_WRLCK,offset,whence,len)
/* 写文件区域加锁(阻塞) */
#define writew_lock(fd,offset,whence,len)   lock_reg(fd,F_SETLKW,F_WRLCK,offset,whence,len)
/* 文件区域解锁 */
#define un_lock(fd,offset,whence,len)       lock_reg(fd,F_SETLK,F_UNLCK,offset,whence,len)
/* 测试读文件区域锁 */
#define is_readlock(fd,offset,whence,len)   lock_test(fd,F_RDLCK,offset,whence,len)
/* 测试写文件区域锁 */
#define is_writelock(fd,offset,whence,len)  lock_test(fd,F_WRLCK,offset,whence,len)

#define FILE_MODE   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define DIR_MODE    (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)


int main(int argc, char* args[])
{
    int fd;
    int flags;
    char buf[10];

    /* 用一把文件锁来保证，一份程序只有一个实例运行 */

    fd = open("daemon.pid", O_WRONLY|O_CREAT, FILE_MODE);
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

    ftruncate(fd, 0);   // 长度截为0
    /* write pid */
    sprintf(buf, "%d\n", getpid());
    if (write(fd, buf, strlen(buf)) != strlen(buf))
        printf("write error\r\n");

    /* 当fork子进程后仍可用fd，FD_CLOEXEC使exec后系统就会关闭子进程中的fd */
    flags = fcntl(fd, F_GETFD);  
    flags |= FD_CLOEXEC;  
    fcntl(fd, F_SETFD, flags);

    // do something...
    while (1)
        pause();

    exit(0);
}



