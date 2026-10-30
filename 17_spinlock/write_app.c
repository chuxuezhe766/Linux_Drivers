#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int fd;
    char str1[10] = {0};
    fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        printf("file open failed\n");
        return -1;
    }
    if (strcmp(argv[2], "Linux") == 0) {
        write(fd, "Linux", strlen("Linux")); 
    }
    else if (strcmp(argv[2], "kernel") == 0) {
        write(fd, "kernel", strlen("kernel"));
    }
    close(fd);
    return 0;
}
