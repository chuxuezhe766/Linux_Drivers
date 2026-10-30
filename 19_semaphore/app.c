#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char *argv[])
{
    int fd;
    char str1[100] = {0};

    if (argc < 3) {
        printf("用法：%s <设备路径> <test|te>\n", argv[0]);
        return -1;
    }

    // Open the file of device
    fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        printf("file open failed\n");
        return -1;
    }

    /* if the second parameter is "test" and it is written to topeet, the driver will
    sleep for 4 susecond*/
    if (strcmp(argv[2], "test") == 0) {
    write(fd, "test", strlen("test"));     /* 或 strlen(argv[2]) */
    }
    else if (strcmp(argv[2], "te") == 0) {
        write(fd, "te", strlen(argv[2]));
    }
    close(fd);
    return 0;
}