#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>

// Define the variable spinlock
static spinlock_t spinlock_test;
static int flag = 1;

// open function
static int open_test(struct inode *inode, struct file *file)
{
    spin_lock(&spinlock_test);
    if (flag != 1) {                /* 设备已被别人占用 */
        spin_unlock(&spinlock_test);
        return -EBUSY;
    }
    flag = 0;                       /* 占用它 */
    spin_unlock(&spinlock_test);
    return 0;
}

static char kbuf[10] = {0};
// write function
static ssize_t cdev_test_write(struct file *file, const char __user *buf, 
                            size_t size, loff_t *off)
{
    if (copy_from_user(kbuf, buf, size) != 0) {
        printk("copy_from_user error\r\n");
        return -1;
    }
    if (strcmp(kbuf, "linux") == 0) {
        ssleep(4);
    }
    else if (strcmp(kbuf, "kernel") == 0) {
        ssleep(2);
    }
    printk("This is cdev_test_write\r\n");
    printk("kubf is %s\r\n", kbuf);
    return 0;
}

// read function
static ssize_t cdev_test_read(struct file *file, char __user *buf, size_t size,
                            loff_t *off)
{
    char kbuf[32] = "This is cdev_test_read!";
    printk("/nthis is read_test\n");
    if (copy_to_user(buf, kbuf, strlen(kbuf)) != 0) {
        printk("copy_to_user error\r\n");
        return -1;
    }
    printk("This is cdev_test_read\r\n");
    return 0;
}

static int release_test(struct inode *inode, struct file *file)
{
    printk("\nthis is release_test\n");
    spin_lock(&spinlock_test);
    flag = 1;
    spin_unlock(&spinlock_test);
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
};

static int __init atomic_init(void)
{
    spin_lock_init(&spinlock_test);
    if (alloc_chrdev_region(&dev1.dev_num, 0, 1, "chrdev_name") < 0) {
        printk("alloc_chrdev_region is error\n");
    }
    printk("alloc_chrdev_region is ok\n");
    dev1.major = MAJOR(dev1.dev_num);
    dev1.minor = MINOR(dev1.dev_num);
    printk("major is %d, minor is %d\n", dev1.major, dev1.minor);
    cdev_init(&dev1.cdev_test, &fops_test);
    dev1.cdev_test.owner = THIS_MODULE;
    cdev_add(&dev1.cdev_test, dev1.dev_num, 1);
    dev1.class_test = class_create(THIS_MODULE, "class_test");
    device_create(dev1.class_test, 0, dev1.dev_num, 0, "device_test");
    return 0;
}

static void __exit atomic_exit(void)
{
    device_destroy(dev1.class_test, dev1.dev_num);
    class_destroy(dev1.class_test);
    cdev_del(&dev1.cdev_test);
    unregister_chrdev_region(dev1.dev_num, 1);
    printk("module exit\n");
}

// Create a module registration macro
module_init(atomic_init);
module_exit(atomic_exit);

// Create a module declaration macro
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("Module Driver");
MODULE_AUTHOR("kongchen");
