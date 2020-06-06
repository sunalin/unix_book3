#include <stdio.h>
#include <string.h>
#include <unistd.h>
 
int main(int argc, char *args[])
{
    /* input_output */

    char buf[1024];

    int c;

    while (1)
    {
        c = getc(stdin);
        if (c == EOF)
            break;
        putc(c, stdout);
    }

    return 0;
}



