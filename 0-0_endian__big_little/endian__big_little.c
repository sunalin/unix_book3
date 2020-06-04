#include <stdio.h>
#include <string.h>
#include <unistd.h>

int mian(int argc, char* args[])
{
    /* ´óÐ¡¶Ë */

    unsigned int abc = 0x04030201;
    char* p = (char*)&abc;

    printf("\n[0x%08x]\n"
           "Low address >> High address:\n"
           "p0[0x%02x]\n"
           "p1[0x%02x]\n"
           "p2[0x%02x]\n"
           "p3[0x%02x]\n\n",
           abc,
           p[0], p[1], p[2], p[3]);

    if (p[0] == 0x01 &&
        p[3] == 0x04)
        printf("little endian \n\n");
    if (p[0] == 0x04 &&
        p[3] == 0x01)
        printf("big endian \n\n");

    return 0;
}



