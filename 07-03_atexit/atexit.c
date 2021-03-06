#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

static void exit_func1()
{
    printf("exit_func1\r\n");
}
static void exit_func2()
{
    printf("exit_func2\r\n");
}
static void exit_func3()
{
    printf("exit_func3\r\n");
}


int main(int argc, char* args[])
{
    /* atexit    注册一个或多个终止处理程序，exit自动调用 */
    atexit(exit_func1);
    atexit(exit_func2);
    atexit(exit_func3);
    atexit(exit_func2);
    atexit(exit_func2);

    printf("main is done\r\n");

    return 0;
}



