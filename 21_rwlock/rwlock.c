#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/rwlock.h>
#include <linux/delay.h>


static char shared_data[256];
static int data_version = 0;
static rwlock_t my_rwlock;

#define RWLOCK_IOC_READ         _IOR('R', 0, char[256])
#define RWLOCK_IOC_WRITE        _IOW('R', 1, char[256])
#define RWLOCK_IOC_VERSION      _IOR('R', 2, int)

// open function
static int open_test(struct inode *inode, struct file *file)
{
    printk("\nthis is open_test \n");
    return 0;
}

// write function
static ssize_t cdev_test_write(struct file *file, const char __user *buf,
                            size_t len, loff_t *off)
{
    char temp[256];
    int bytes_to_write;

    bytes_to_write = min(len, (size_t)(sizeof(shared_data) - 1));
    
    if (copy_from_user(temp, buf, bytes_to_write)) {
        return -EFAULT;
    }
    temp[bytes_to_write] = '\0';

    write_lock(&my_rwlock);
    printk(KERN_INFO "[rwlock_demo] 写者(PID:%d) 获取写锁， 正在修改数据...\n",
            current->pid);

    mdelay(100);

    memcpy(shared_data, temp, bytes_to_write);
    shared_data[bytes_to_write] = '\0';
    data_version ++;

    printk(KERN_INFO "[rwlock_demo] 写者(PID:%d) 数据已经更新为: %s (版本:%d)\n",
            current->pid, shared_data, data_version);

    write_unlock(&my_rwlock);

    printk(KERN_INFO "[rwlock_demo] 写者(PID:%d) 释放写锁\n",
        current->pid);
    return bytes_to_write;
}

// read function
static ssize_t cdev_test_read(struct file *file, char __user *buf, size_t len,
                            loff_t *off)
{
    char temp[256];
    int str_len;

    read_lock(&my_rwlock);
    printk(KERN_INFO "[rwlock_demo] 读者(PID:%d) 获取读锁， 正在读取数据...\n",
        current->pid);

    mdelay(100);
    str_len = snprintf(temp, sizeof(temp), "版本：%d 数据:%s", data_version, shared_data);

    read_unlock(&my_rwlock);
    printk(KERN_INFO "[rwlock_demo] 读者(PID:%d) 释放读锁\n",
        current->pid);

    if (str_len > len) {
        str_len = len;
    }
    if (copy_to_user(buf, temp, str_len)) {
        return -EFAULT;
    }

    *off += str_len;
    return str_len;
    
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    long ret = 0;

    switch (cmd) {
    case RWLOCK_IOC_READ: {                 /* 大括号内声明,只属于这个 case */
        char temp[256];
        int len;

        read_lock(&my_rwlock);
        len = snprintf(temp, sizeof(temp), "版本:%d 数据:%s",
                    data_version, shared_data);
        read_unlock(&my_rwlock);

        if (copy_to_user((char __user *)arg, temp, len + 1))
            ret = -EFAULT;
        break;
    }

    case RWLOCK_IOC_WRITE: {                /* WRITE 用自己的 local,和 temp 无关 */
        char local[256];

        if (copy_from_user(local, (char __user *)arg, sizeof(local) - 1)) {
            ret = -EFAULT;
            break;
        }
        local[sizeof(local) - 1] = '\0';

        write_lock(&my_rwlock);             /* 锁内只做内存拷贝,不做用户态访问 */
        memcpy(shared_data, local, sizeof(shared_data));
        data_version++;
        write_unlock(&my_rwlock);
        break;
    }

    case RWLOCK_IOC_VERSION: {
        int value;

        read_lock(&my_rwlock);
        value = data_version;
        read_unlock(&my_rwlock);

        if (copy_to_user((int __user *)arg, &value, sizeof(value)))
            ret = -EFAULT;
        break;
    }

    default:
        ret = -ENOTTY;
        break;
    }

    return ret;
}

static int release_test(struct inode *inode, struct file *file)
{
    printk("\nthis is release_test \n");
    return 0;
}

struct chrdev_test {
    dev_t dev_num;
    int major, minor;
    struct cdev cdev_test;
    struct class *class_test;
};
struct chrdev_test dev1;

struct file_operations fops_test = {
    .owner = THIS_MODULE,
    .open = open_test,
    .read = cdev_test_read,
    .write = cdev_test_write,
    .release = release_test,
    .unlocked_ioctl = my_ioctl,
};

static int __init rwlock_demo_init(void)
{
    int ret;

    // Automatically obtain the device number
    ret = alloc_chrdev_region(&dev1.dev_num, 0, 1, "rwlock_demo");
    if (ret < 0) {
        printk(KERN_ERR "[rwlock_demo] 分配设备号失败:%d\n", ret);
        return ret;
    }
    printk("alloc_chrdev_region is ok\n");

    // Obtain the major device number using MAJOR function    
    dev1.major = MAJOR(dev1.dev_num);
    dev1.minor = MINOR(dev1.dev_num);
    // Obtain the minor device number using MINOR function
    printk("major is %d, minor is %d\n", dev1.major, dev1.minor);

    // Initialize the cdev test structure using the cdev init funciton
    cdev_init(&dev1.cdev_test, &fops_test);
    dev1.cdev_test.owner = THIS_MODULE;
    // Add the cdev test    
    cdev_add(&dev1.cdev_test, dev1.dev_num, 1);
    // Creat the class and device
    dev1.class_test = class_create(THIS_MODULE, "class_test");
    device_create(dev1.class_test, 0, dev1.dev_num, 0, "device_test");

    // Initialize read ang write lock
    rwlock_init(&my_rwlock);
    memset(shared_data, 0, sizeof(shared_data));
    data_version = 0;

    printk(KERN_INFO "[rwlock_demo] 模块加载成功，读写锁已经初始化\n");
    return 0;
}

static void __exit rwlock_demo_exit(void)
{
    device_destroy(dev1.class_test, dev1.dev_num);
    class_destroy(dev1.class_test);
    cdev_del(&dev1.cdev_test);
    unregister_chrdev_region(dev1.dev_num, 1);
    printk("module exit\n");
}

// Create a module registration macro
module_init(rwlock_demo_init);
module_exit(rwlock_demo_exit);

// Create a module declaration macro
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("Module Driver");
MODULE_AUTHOR("kongchen");
