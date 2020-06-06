#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
 
int main(int argc, char *args[])
{
    /* lseek …Ë÷√∆´“∆¡ø */

    int pos = -1;
    pos = lseek(STDIN_FILENO, 0, SEEK_CUR);
    printf("lseek return value:%d\r\n", pos);


    return 0;
}



