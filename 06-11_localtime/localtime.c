#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char* args[])
{
    time_t t;
    char buf[1024];

    t = time(0);
    /* strftime    格式化时间 */
    strftime(buf, sizeof(buf),
             "date: [%Y-%m-%d]%t%t time: [%H:%M:%S]%n"
             "date: [%A %B %d %Y]%t time: [%r]",
             localtime(&t));
    printf("%s\r\n", buf);
    
    return 0;
}



