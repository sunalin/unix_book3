#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

 
int main(int argc, char *args[])
{
    /* read write */

    int n;
    char buf[4096];

    int fd = -1;

    while (1)
    {
        n = read(STDIN_FILENO, buf, sizeof(buf));
        if (n < 0)
            printf("read error");
        else if (n > 0)
        {
            if (write(STDOUT_FILENO, buf, n) != n)
                printf("write error");
        }
    }

    return 0;
}



