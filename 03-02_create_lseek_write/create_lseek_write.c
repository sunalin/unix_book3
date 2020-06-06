#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

 
int main(int argc, char *args[])
{
    /* create lseek write */

    char buf1[] = "abcdefg";
    char buf2[] = "ABCDEFG";

    int fd = -1;

    // 可使用open代替creat
    //fd = creat("file.hole", FILE_MODE);
    fd = open("file.hole", O_WRONLY | O_CREAT | O_TRUNC);
    write(fd, buf1, strlen(buf1));
    lseek(fd, 6, SEEK_CUR);
    write(fd, buf2, strlen(buf2));

    //od -c file.hole

    return 0;
}




