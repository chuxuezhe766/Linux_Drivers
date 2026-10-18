#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int fd;
    char buf1[32] = {0};

    fd = open("/dev/test", O_RDWR | O_NONBLOCK);
    if (fd < 0) {
        perror("open error\n");
        return fd;
    }

    printf("read before\n");

    // Do forever
    while (1) {
        int ret = read(fd, buf1, sizeof(buf1));
        if (ret > 0) {
            printf("buf is %s\n", buf1);
            break;
        } 
        else if (ret < 0) {
            printf("data not ready, retry...\n");
            sleep(1);
        }
        else {
            printf("read returns 0\n");
            sleep(1);
        }
    }

    printf("read after\n");

    close(fd);
    return 0;
}
