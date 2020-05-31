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
    pid_t pid;
    struct stat sbuf;
    char* fifo_path = "./temp.fifo";


    if (access(fifo_path, F_OK) == -1       /* �ļ������� */
        || stat(fifo_path, &sbuf) == -1
        || !S_ISFIFO(sbuf.st_mode))       
    {
        /* mkfifo    ����fifo�����ܵ��ļ��������ڲ���ؽ���ͨ�ţ�ÿ������ͨ��fifo�ļ�����

           O_RDONLY: open��������һ��������д��ʽ��fifo
           O_WRONLY: open��������һ�������Զ���ʽ��fifo
           O_RDONLY|O_NONBLOCK: open�����򿪣���������fd
           O_WRONLY|O_NONBLOCK: �Ѿ���һ�������Զ���ʽ��fifo����������fd�����򷵻�-1 */
           
        remove(fifo_path);
        if (mkfifo(fifo_path, 0777) != 0)
            perror("mkfifo");
        else
            printf("mkfifo: %s\r\n", fifo_path);
    }


    pid = fork();
    if (pid > 0)
    {
        /* parent process */
        char dat_buf[100];
        int fd_fifo;

        fd_fifo = open(fifo_path, O_WRONLY);            /* ֻд������FIFO�ļ� */
        //fd_fifo = open(fifo_path, O_WRONLY|O_NONBLOCK); /* ֻд��������FIFO�ļ� */
        memset(dat_buf, 'a', sizeof(dat_buf));
        dat_buf[sizeof(dat_buf) - 1] = '\0';
        write(fd_fifo, dat_buf, sizeof(dat_buf));   /* дFIFO */

        waitpid(pid, 0, 0);        
        close(fd_fifo);
    }
    else if (pid == 0)
    {
        /* child process */
        char dat_buf[100];
        int fd_fifo;

        fd_fifo = open(fifo_path, O_RDONLY);            /* ֻ��������FIFO�ļ� */
        //fd_fifo = open(fifo_path, O_RDONLY|O_NONBLOCK); /* ֻ����������FIFO�ļ� */
        read(fd_fifo, dat_buf, sizeof(dat_buf));   /* ��FIFO */
        dat_buf[sizeof(dat_buf) - 1] = '\0';
        printf("%s\r\n", dat_buf);

        close(fd_fifo);
    }

    return 0;
}



