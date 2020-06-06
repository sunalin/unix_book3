#include <stdio.h>
#include <string.h>
#include <unistd.h>
 
int main(int argc, char *args[])
{
    /* input_output */

    char buf[1024];

    while (1)
    {
        char info[100] = "input:";
        write(STDOUT_FILENO, info, sizeof(info));

        memset(buf, 0, sizeof(buf));
        read(STDIN_FILENO, buf, sizeof(buf));
        
        write(STDOUT_FILENO, buf, strlen(buf));
    }	

    return 0;
}



