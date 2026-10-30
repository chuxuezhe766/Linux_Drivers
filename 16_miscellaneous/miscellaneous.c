#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

// write function
static ssize_t cdev_test_write(struct file *file, const char __user *buf, 
                            size_t size, loff_t *off)
{
    char kbuf[32] = {0};
    if (copy_from_user(kbuf, buf, size) != 0) {
        printk("copy_from_user error\r\n");
        return -1;
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
    if (copy_to_user(buf, kbuf, strlen(kbuf)) != 0) {
        printk("copy_to_user error\r\n");
        return -1;
    }
    printk("This is cdev_test_read\r\n");
    return 0;
}

struct file_operations misc_fops = {
    .owner = THIS_MODULE,
    .read = cdev_test_read,
    .write = cdev_test_write,
};
struct miscdevice misc_dev = {
    .minor = MISC_DYNAMIC_MINOR,    // dynamic apply for the device number
    .name = "miscdriver",           // the device name
    .fops = &misc_fops,
};

static int __init misc_init(void)
{
    int ret;
    ret = misc_register(&misc_dev);
    if (ret < 0) {
        printk("misc registe is error\n");
    }
    printk("misc registe is succeed\n");
    return 0;
}

static void __exit misc_exit(void)
{
    misc_deregister(&misc_dev);
    printk("misc goodby!\n");
}

// Create a module registration macro
module_init(misc_init);
module_exit(misc_exit);

// Create a module declaration macro
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("Module Driver");
MODULE_AUTHOR("kongchen");
