#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <pthread.h>

#define DEVICE              "/dev/device_test"

/* 和驱动 rwlock.c 里的命令宏一字不差 */
#define RWLOCK_IOC_READ     _IOR('R', 0, char[256])
#define RWLOCK_IOC_WRITE    _IOW('R', 1, char[256])
#define RWLOCK_IOC_VERSION  _IOR('R', 2, int)

#define READER_NUM  3       /* 读者线程数 */
#define WRITER_NUM  2       /* 写者线程数 */
#define READ_TIMES  5       /* 每个读者读几次 */
#define WRITE_TIMES 3       /* 每个写者写几次 */

/* 读者线程:只做 ioctl 读(多读者可并行持读锁) */
void *reader_thread(void *arg)
{
    int id = *(int *)arg;
    char buf[256];
    int i;
    int fd = open(DEVICE, O_RDWR);

    if (fd < 0) {
        perror("[读者] open");
        return NULL;
    }

    for (i = 0; i < READ_TIMES; i++) {
        memset(buf, 0, sizeof(buf));

        if (ioctl(fd, RWLOCK_IOC_READ, buf) == 0)
            printf("[读者%d] 第%d次读取: %s\n", id, i + 1, buf);
        else
            perror("[读者] ioctl READ");

        usleep(200000);          /* 200ms 一轮 */
    }

    close(fd);
    printf("[读者%d] 退出\n", id);
    return NULL;
}

/* 写者线程:做 ioctl 写(写者独占,和读者互斥) */
void *writer_thread(void *arg)
{
    int id = *(int *)arg;
    char buf[256];
    int i;
    int fd = open(DEVICE, O_RDWR);

    if (fd < 0) {
        perror("[写者] open");
        return NULL;
    }

    for (i = 0; i < WRITE_TIMES; i++) {
        snprintf(buf, sizeof(buf), "写者%d-第%d次写入", id, i + 1);

        if (ioctl(fd, RWLOCK_IOC_WRITE, buf) == 0)
            printf("[写者%d] 第%d次写入完成: %s\n", id, i + 1, buf);
        else
            perror("[写者] ioctl WRITE");

        usleep(500000);          /* 500ms 一轮 */
    }

    close(fd);
    printf("[写者%d] 退出\n", id);
    return NULL;
}

int main(void)
{
    pthread_t readers[READER_NUM];
    pthread_t writers[WRITER_NUM];
    int reader_ids[READER_NUM];
    int writer_ids[WRITER_NUM];
    int i;

    printf("---------- 读写锁测试 -----------\n");
    printf("启动 %d 个读者线程(每200ms读一次)\n", READER_NUM);

    /* 创建读者线程 */
    for (i = 0; i < READER_NUM; i++) {
        reader_ids[i] = i + 1;
        pthread_create(&readers[i], NULL, reader_thread, &reader_ids[i]);
    }

    /* 稍微延迟后创建写者线程 */
    usleep(100000);
    printf("启动 %d 个写者线程(每500ms写一次)\n", WRITER_NUM);
    for (i = 0; i < WRITER_NUM; i++) {
        writer_ids[i] = i + 1;
        pthread_create(&writers[i], NULL, writer_thread, &writer_ids[i]);
    }

    /* 等待所有线程结束 */
    for (i = 0; i < READER_NUM; i++) {
        pthread_join(readers[i], NULL);
    }
    for (i = 0; i < WRITER_NUM; i++) {
        pthread_join(writers[i], NULL);
    }

    printf("\n========== 测试完成 ==========\n");

    /* 最终读取一次数据版本 */
    int fd = open(DEVICE, O_RDWR);
    if (fd >= 0) {
        int version;
        ioctl(fd, RWLOCK_IOC_VERSION, &version);
        printf("最终数据版本: %d\n", version);
        close(fd);
    }

    return 0;
}
