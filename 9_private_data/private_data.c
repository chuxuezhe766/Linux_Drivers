#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

struct device_test {
    dev_t dev_num;      // device number
    int major;          // the variable major
    int minor;          // the variable minor
    struct cdev cdev_test;  // cdev
    struct class *class;    // class
    struct device *device;  // device
    char kbuf[32];          // buf
};

// the variable dev1
struct device_test dev1;

// Open
static int cdev_test_open(struct inode *inode, struct file *file)
{
    file->private_data = &dev1;     // Set private data
    printk("This is cdev_test_open\r\n");
    return 0;
}

// Write
static ssize_t cdev_test_write(struct file *file, const char __user *buf, size_t size, loff_t *off)
{
    struct device_test *test_dev = (struct device_test *)file->private_data;
    if (copy_from_user(test_dev->kbuf, buf, size) != 0) {
        printk("copy_from_user error\r\n");
        return -1;
    }
    printk("This is cdev_test_write\r\n");

    printk("kbuf is %s\r\n", test_dev->kbuf);
    return 0;
}

// Read 
static ssize_t cdev_test_read(struct file *file, char __user *buf, size_t size, loff_t *off)
{
    struct device_test *test_dev = (struct device_test *)file->private_data;
    if (copy_to_user(buf, test_dev->kbuf, strlen(test_dev->kbuf)) != 0)
    {
        printk("copy_to_user error\r\n");
        return -1;
    }

    printk("This is cdev_test_read\r\n");
    return 0;
}

// Release 
static int cdev_test_release(struct inode *inode, struct file *file)
{
    printk("This is cdev_test_release\r\n");
    return 0;
}

// File operations
struct file_operations cdev_test_fops = {
    .owner = THIS_MODULE,
    .open = cdev_test_open,
    .read = cdev_test_read,
    .write = cdev_test_write,
    .release = cdev_test_release,
};

// Driver entry function
static int __init chr_fops_init(void)
{
    int ret;
    ret = alloc_chrdev_region(&dev1.dev_num, 0, 1, "alloc_name");
    if (ret < 0) {
        printk("alloc_chrdev_region is error\n");
    }
    printk("alloc_chrdev_region is ok\n");
    dev1.major = MAJOR(dev1.dev_num);
    dev1.minor = MINOR(dev1.dev_num);

    printk("major is %d\r\n", dev1.major);
    printk("minor is %d\r\n", dev1.minor);

    cdev_init(&dev1.cdev_test, &cdev_test_fops);
    dev1.cdev_test.owner = THIS_MODULE;

    cdev_add(&dev1.cdev_test, dev1.dev_num, 1);

    dev1.class = class_create(THIS_MODULE, "test");
    dev1.device = device_create(dev1.class, NULL, dev1.dev_num, NULL, "test");

    return 0;
}

// Driver exit function
static void __exit chr_fops_exit(void)
{
    // Delete chrdev device
    unregister_chrdev_region(dev1.dev_num, 1);
    cdev_del(&dev1.cdev_test);
    device_destroy(dev1.class, dev1.dev_num);
    class_destroy(dev1.class);
}

// Create a module registration macro
module_init(chr_fops_init);
module_exit(chr_fops_exit);

// Create a module declaration macro
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("Module Driver");
MODULE_AUTHOR("kongchen");