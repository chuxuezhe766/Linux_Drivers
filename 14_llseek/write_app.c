#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int fd;
    char buf1[32] = "hello";
    char buf2[32] = "world";
    char read_buf[64] = {0};

    fd = open("/dev/test", O_RDWR);

    if (fd < 0) {
        perror("open error \n");
        return fd;
    }

    write(fd, buf1, strlen(buf1));
    printf("write: %s\n", buf1);

    lseek(fd, 5, SEEK_SET);

    write(fd, buf2, strlen(buf2));
    printf("write: %s\n", buf2);

    lseek(fd, 0, SEEK_SET);

    read(fd, read_buf, sizeof(read_buf));
    printf("read: %s\n", read_buf);

    close(fd);
    return 0;
}
