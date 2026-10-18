#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    // Define the variable fd;
    int fd;
    // Define the variable buf;
    char buf[32];
    // Call the open function
    fd = open(argv[1], O_RDWR, 0666);
    if (fd < 0) {
        printf("open is error\n");
        return -1;
    }
    printf("open is ok\n");
    // Call the read function
    if (!strcmp(argv[2], "read")) {
        read(fd, buf, 32);
    }
    else if (!strcmp(argv[2], "write")) {
        write(fd, "hello\n", 6);
    }
    // Call the close function
    close(fd);
    return 0;
}
