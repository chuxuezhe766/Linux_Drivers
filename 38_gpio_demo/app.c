/* 37_gpio_control: sysfs GPIO 输出实验
 * GPIO1_A4 = 1*32 + 0*8 + 4 = 36（vccio1 = 3.0V 域）
 * 编译: aarch64-none-linux-gnu-gcc app.c -o app
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

static int gpio_export(int gpio_num)
{
    char buf[64];
    int fd, ret;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0) {
        perror("open export");
        return -1;
    }
    snprintf(buf, sizeof(buf), "%d", gpio_num);
    ret = write(fd, buf, strlen(buf));
    close(fd);
    if (ret < 0) {
        if (errno == EBUSY) {
            /* 已被借出（比如 shell 手动 export 过）——不算错 */
            printf("gpio%d 已 export，复用\n", gpio_num);
            return 0;
        }
        perror("write export");
        return -1;
    }
    return 0;
}

static int gpio_unexport(int gpio_num)
{
    char buf[64];
    int fd, ret;

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd < 0) {
        perror("open unexport");
        return -1;
    }
    snprintf(buf, sizeof(buf), "%d", gpio_num);
    ret = write(fd, buf, strlen(buf));
    close(fd);
    if (ret < 0) {
        perror("write unexport");
        return -1;
    }
    return 0;
}

static int gpio_set_direction(int gpio_num, const char *dir)
{
    char path[128];
    int fd, ret;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", gpio_num);
    fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror("open direction");
        return -1;
    }
    ret = write(fd, dir, strlen(dir));
    close(fd);
    if (ret < 0) {
        perror("write direction");
        return -1;
    }
    return 0;
}

static int gpio_set_value(int gpio_num, int val)
{
    char path[128];
    int fd, ret;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", gpio_num);
    fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror("open value");
        return -1;
    }
    ret = write(fd, val ? "1" : "0", 1);
    close(fd);
    if (ret < 0) {
        perror("write value");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int gpio_num = 36;  /* GPIO1_A4 = 1*32 + 0*8 + 4 = 36 */

    if (gpio_export(gpio_num) < 0)
        return -1;

    if (gpio_set_direction(gpio_num, "out") < 0)
        return -1;

    if (gpio_set_value(gpio_num, 1) < 0)
        return -1;
    printf("GPIO%d 亮 2 秒...\n", gpio_num);
    fflush(stdout);
    sleep(2);

    if (gpio_set_value(gpio_num, 0) < 0)
        return -1;

    gpio_unexport(gpio_num);
    printf("完成，引脚已归还\n");
    return 0;
}
