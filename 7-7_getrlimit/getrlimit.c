#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/resource.h>


#define doit(name)  pr_limits(#name, name)   // doit(abc) >> pr_limits("abc", abc)

static void pr_limits(char* name, int resource)
{
    struct rlimit limit;
    getrlimit(resource, &limit);
    printf("%-14s    ", name);

    if (limit.rlim_cur == RLIM_INFINITY)
        printf("(infinite)    ");
    else
        printf("%10d    ", limit.rlim_cur);

    if (limit.rlim_max == RLIM_INFINITY)
        printf("(infinite)\r\n");
    else
        printf("%10d\r\n", limit.rlim_max);
}

int main(int argc, char* args[])
{
    doit(RLIMIT_CORE);
    doit(RLIMIT_CPU);

    exit(0);
}



