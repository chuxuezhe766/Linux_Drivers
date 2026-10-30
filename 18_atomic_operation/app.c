#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define DEVICE_PATH "/dev/device_test"

int main(int argc, char *argv[])
{
    int fd;
    int ret;
    char write_buf[64];
    char read_buf[64];

    if (argc < 2) {
        printf("用法： %s <命令>\n", argv[10]);
        printf("open - 打开设备\n");
        printf("read - 读取设备\n");
        printf("write - 写入数据（需要第二个参数）\n");
        printf("close - 关闭设备（通过写入模拟）\n");
        return -1;
    }

    if (strcmp(argv[1], "open") == 0) {
        fd = open(DEVICE_PATH, O_RDWR);
        if (fd < 0) {
            if (errno == EBUSY) {
                printf("设备忙， 已被其他进程占用\n");
            }
            else {
                perror("open");
            }
            return -1;
        }
        printf("设备打开成功， fd = %d\n", fd);
        printf("按回车键关闭设备");
        getchar();
        close(fd);
        printf("设备已经关闭");
    }
    else if (strcmp(argv[1], "read") == 0) {
        fd = open(DEVICE_PATH, O_RDWR);
        if (fd < 0) {
            perror("open");
            return -1;
        }

        // Read data from the device
        ret = read(fd, read_buf, sizeof(read_buf));
        if (ret >= 0) {
            printf("读取到数据：%s\n", read_buf);
        }
        close(fd);
    }
    else if(strcmp(argv[1], "write") == 0) {
        if (argc < 3) {
            printf("请提供写入数据， 如：%s write topeet\n", argv[0]);
            return -1;
        }

        fd = open(DEVICE_PATH, O_RDWR);
        if (fd < 0) {
            perror("open");
            return -1;
        }
        strncpy(write_buf, argv[2], sizeof(write_buf) - 1);
        printf("正在写入：%s\n", write_buf);
        write(fd, write_buf, strlen(write_buf));
        printf("写入完成");
        close(fd);
    }

    return 0;
}