#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    // Define the variable fd
    int fd;
    // Define the variable buf1
    char buf1[32] = {0};
    char buf2[32] = "nihao";
    fd = open("/dev/test", O_RDWR);
    if (fd < 0) {
        perror("open error\n");
        return fd;
    }
    read(fd, buf1, sizeof(buf1));
    printf("buf1 is %s \r\n", buf1);

    write(fd, buf2, sizeof(buf2));
    close(fd);
    return 0;
}
