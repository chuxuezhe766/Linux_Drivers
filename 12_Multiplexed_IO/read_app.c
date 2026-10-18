#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>

int main(int argc, char *argv[])
{
    int fd;
    char buf1[32] = {0};
    struct pollfd fds[1];
    int ret;

    fd = open("/dev/test", O_RDWR);
    if (fd < 0) {
        perror("open error\n");
        return fd;
    }

    // Set the variable poll 
    fds[0].fd = fd;
    fds[0].events = POLLIN;

    printf("read before\n");

    // Do forever
    while (1) {
        ret = poll(fds, 1, 3000);
        if (!ret) {
            printf("time out!!!\n");
        }
        else if (fds[0].revents & POLLIN) {
            read(fd, buf1, sizeof(buf1));
            printf("buf is %s\n", buf1);
            break;
        }
    }

    printf("read after\n");

    close(fd);
    return 0;
}
