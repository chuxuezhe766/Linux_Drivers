#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/atomic.h>

// Define the atomic variable 
static atomic64_t v = ATOMIC64_INIT(1);

// open function
static int open_test(struct inode *inode, struct file *file)
{
    // Read the atomic variable
    if (atomic64_read(&v) != 1) {
        return -EBUSY; 
    }

    atomic64_set(&v, 0);
    return 0;
}

static char kbuf[10] = {0};
// write function
static ssize_t cdev_test_write(struct file *file, const char __user *buf, 
                            size_t len, loff_t *off)
{
    int ret;
    ret = copy_from_user(kbuf, buf, len);
    if (ret != 0) {
        printk("copy_from_user is error\n");
    }

    if (strcmp(kbuf, "topeet") == 0) {
        ssleep(4);
    }
    else if (strcmp(kbuf, "itop") == 0) {
        ssleep(2);
    }

    printk("copy_from_user buf is %s\n", kbuf);
    return 0;
}

// read function
static ssize_t cdev_test_read(struct file *file, char __user *buf, size_t len,
                            loff_t *off)
{
    int ret;
    char kbuf[10] = "topeet";
    printk("\nthis is read_test\n");

    ret = copy_to_user(buf, kbuf, strlen(kbuf));
    if (ret != 0) {
        printk("copy_to_user is error\n");
    }
    printk("copy_to user is ok\n");
    return 0;
}

static int release_test(struct inode *inode, struct file *file)
{
    // Restore the atomic variable to 1
    atomic64_set(&v, 1);
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
    // Automatically obtain the device number
    if (alloc_chrdev_region(&dev1.dev_num, 0, 1, "chrdev_name") < 0) {
        printk("alloc_chrdev_region is error\n");
        return -1;
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
