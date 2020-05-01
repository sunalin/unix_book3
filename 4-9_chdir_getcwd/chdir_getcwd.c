#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


int main(int argc, char* args[])
{
    char* ptr;
    int ptr_size = 100;

    char new_work_dir[] = "../";

    ptr = (char*)malloc(ptr_size);

    // ��ǰ����Ŀ¼
    if (getcwd(ptr, ptr_size) == NULL)
    {
        printf("getcwd error\r\n");
        return 0;
    }
    printf("[current work dir] = %s\r\n", ptr);

    // ���Ĺ���Ŀ¼
    if (chdir(new_work_dir) < 0)
    {
        printf("chdir error\r\n");
        return 0;
    }
    printf("[change work dir]  = %s\r\n", new_work_dir);

    // ��ǰ����Ŀ¼
    if (getcwd(ptr, ptr_size) == NULL)
    {
        printf("getcwd error\r\n");
        return 0;
    }
    printf("[current work dir] = %s\r\n", ptr);
    
    return 0;
}



