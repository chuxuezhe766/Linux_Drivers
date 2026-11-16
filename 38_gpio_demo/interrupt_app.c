#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>

static int gpio_wait_interrupt(int gpio_num)
{
    char path[128], buf[4];
    struct pollfd fds[1];
    int fd, ret;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", gpio_num);
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open value");
        return -1;
    }

    /* 先读一次：清掉 export 到现在之间积压的事件 */
    read(fd, buf, sizeof(buf));

    fds[0].fd = fd;
    fds[0].events = POLLPRI;

    while (1) {
        ret = poll(fds, 1, -1);            /* 阻塞等待电平沿 */
        if (ret > 0 && (fds[0].revents & POLLPRI)) {
            lseek(fd, 0, SEEK_SET);
            read(fd, buf, sizeof(buf));
            printf("GPIO interrupt! value = %c\n", buf[0]);
            fflush(stdout);
        }
    }

    close(fd);
    return 0;
}

int main(void)
{
    gpio_wait_interrupt(36);
    return 0;
}

