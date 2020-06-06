#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

 
int main(int argc, char *args[])
{
    /* dup dup2 复制文件描述符 */

    char buf1[] = "abcdefg";
    char buf2[] = "ABCDEFG";

    int fd = -1;
    int new_fd = -1;

    // 可使用open代替creat
    //fd = creat("file.hole", FILE_MODE);
    fd = open("file.hole", O_WRONLY | O_CREAT | O_TRUNC);

    // new_fd可理解成是fd别名，lseek时也会同步更新，操作new_fd就相关于操作fd
    new_fd = dup(fd);
    write(fd, buf1, strlen(buf1));
    lseek(new_fd, 6, SEEK_CUR);
    write(new_fd, buf2, strlen(buf2));

    //od -c file.hole


    return 0;
}



