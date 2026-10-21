#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int fd;
char buf1[32] = {0};

static void func(int signum)
{
    read(fd, buf1, 32);
    printf("buf is %s\n", buf1);
}

int main(int argc, char *argv[])
{
    int ret;
    int flags;

    fd = open("/dev/test", O_RDWR);
    if (fd < 0) {
        perror("open error\n");
        return fd;
    }

    signal(SIGIO, func);

    fcntl(fd, F_SETOWN, getpid());

    flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | FASYNC);

    while (1) {
        sleep(1);
    }

    close(fd);
    return 0;
}
