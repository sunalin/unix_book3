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


#define FILE_MODE   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define DIR_MODE    (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)

int main(int argc, char* args[])
{
    int fdin;
    int fdout;
    char* src;
    char* dst;
    struct stat statbuf;

    if (argc != 3)
    {
        printf("usage:./mmap <srcfile> <dstfile>\r\n");
        exit(0);
    }

    fdin = open(args[1], O_RDONLY);
    if (fdin < 0)
    {
        printf("open error: %s\r\n", args[1]);
        exit(0);
    }
    fdout = open(args[2], O_RDWR|O_CREAT|O_TRUNC, FILE_MODE);
    if (fdout < 0)
    {
        printf("open error: %s\r\n", args[2]);
        exit(0);
    }

    fstat(fdin, &statbuf);  // file size
    lseek(fdout, statbuf.st_size-1, SEEK_SET);
    write(fdout, "", 1);

    src = mmap(0, statbuf.st_size, PROT_READ,
               MAP_FILE|MAP_SHARED, fdin, 0);
    dst = mmap(0, statbuf.st_size, PROT_READ|PROT_WRITE,
               MAP_FILE|MAP_SHARED, fdout, 0);
    if (!src || !dst)
    {
        printf("mmap error\r\n");
        exit(0);
    }

    memcpy(dst, src, statbuf.st_size);
    
    printf("copy success\r\n");

    /*
       ./mmap mmap.c temp.out
     */

    exit(0);
}



