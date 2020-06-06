#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *args[])
{
    /* errno ¥Ú”°¥ÌŒÛ–≈œ¢ strerror() perror() */

    fprintf(stderr, "EACCES: %s\r\n", strerror(EACCES));
    fprintf(stderr, "ENOENT: %s\r\n", strerror(ENOENT));

    errno = ENOENT;
    perror("sunalin");

    return 0;
}



