#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE "/dev/ioctl_test"

// the macro
#define IOCTL_MAGIC 'M'
#define IOCTL_RESET         _IO(IOCTL_MAGIC, 0)
#define IOCTL_GET_VER       _IOR(IOCTL_MAGIC, 1, int)
#define IOCTL_SET_MODE      _IOW(IOCTL_MAGIC, 2, int)
#define IOCTL_GET_STATUS    _IOR(IOCTL_MAGIC, 3, int)

struct ioctl_data {
    char name[32];
    int value;
};

#define IOCTL_SET_DATA      _IOW(IOCTL_MAGIC, 4, struct ioctl_data)
#define IOCTL_GET_DATA      _IOR(IOCTL_MAGIC, 5, struct ioctl_data)

int main(void)
{
    int fd;
    int version, mode, status;
    struct ioctl_data data;

    // Open device 
    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }
    printf("Device opened\r\n");

    // Obtain the device number
    if (ioctl(fd, IOCTL_GET_VER, &version) < 0) {
        perror("ioctl GET_VER");
        close(fd);
        return -1;
    }
    printf("1. Get version: %d (v%d.%02d)\n", version, version/100, version%100);

    // Set mode 
    mode = 2;
    if (ioctl(fd, IOCTL_SET_MODE, &mode) < 0) {
        perror("ioctl SET_MODE");
        close(fd);
        return -1;
    }
    printf("2. Set mode to: %d\n", mode);

    // Get status
    if (ioctl(fd, IOCTL_GET_STATUS, &status) < 0) {
        perror("ioctl GET_STATUS");
        close(fd);
        return -1;
    }
    printf("3. Get status: %d\n", status);

    // Set data
    strncpy(data.name, "Test Data", sizeof(data.name));
    data.value = 42;
    if (ioctl(fd, IOCTL_SET_DATA, &data) < 0) {
        perror("ioctl SET_DATA");
        close(fd);
        return -1;
    }
    printf("Set data: name=%s, value=%d\n", data.name, data.value);

    // Obtain data
    memset(&data, 0, sizeof(data));
    if (ioctl(fd, IOCTL_GET_DATA, &data) < 0) {
        perror("ioctl GET_DATA");
        close(fd);
        return -1;
    }
    printf("Get data: name=%s, value=%d\n", data.name, data.value);

    // Reset device 
    if (ioctl(fd, IOCTL_RESET) < 0) {
        perror("ioctl RESET");
        close(fd);
        return -1;
    }
    printf("Device reset\n");

    // Check Reset
    if (ioctl(fd, IOCTL_GET_STATUS, &status) < 0) {
        perror("ioctl GET_STATUS");
        close(fd);
        return -1;
    }
    printf("Status after reset: %d\n", status);
    close(fd);
    printf("All ioctl tests passed\n");
    return 0;
}