#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/wait.h>
#include <linux/poll.h>

struct device_test {
    dev_t dev_num;          // the device number
    int major;              // the variable major
    int minor;              // the variable minor
    struct cdev cdev_test;  // cdev
    struct class *class;    // class
    struct device *device;  // device
    char kbuf[32];          // the variable kbuf
    int flag;               // the variable flag
};

// the variable dev1
struct device_test dev1;

// Define and Initialize the wait queue head
DECLARE_WAIT_QUEUE_HEAD(read_wq);

// Open function
static int cdev_test_open(struct inode *inode, struct file *file)
{
    // Set private data
    file->private_data = &dev1;
    printk("This is cdev_test_open\r\n");
    return 0;
}

// Write function
static ssize_t cdev_test_write(struct file *file, const char __user *buf, size_t size, loff_t *off)
{
    struct device_test *test_dev = (struct device_test *)file->private_data;

    if (copy_from_user(test_dev->kbuf, buf, size) != 0) {
        printk("copy_from_user error\r\n");
        return -1;
    }

    // Set the flag to 1
    test_dev->flag = 1;
    // wake up process
    wake_up_interruptible(&read_wq);
    return 0;
}

// Read function
static ssize_t cdev_test_read(struct file *file, char __user *buf, size_t size, loff_t *off)
{
    struct device_test *test_dev = (struct device_test *)file->private_data;
    
    // Nonblocking mode
    if (file->f_flags & O_NONBLOCK) {
        if (test_dev->flag != 1) {
            return -EAGAIN;
        }
    }

    // Make process to sleep
    wait_event_interruptible(read_wq, test_dev->flag);

    if (copy_to_user(buf, test_dev->kbuf, strlen(test_dev->kbuf)) != 0 ) {
        printk("copy_to_user error\r\n");
        return -1;
    }

    test_dev->flag = 0;
    return strlen(test_dev->kbuf);
}

// Release function
static int cdev_test_release(struct inode *inode, struct file *file)
{
    return 0;
}

// Poll function
static __poll_t cdev_test_poll(struct file *file, struct poll_table_struct *p)
{
    struct device_test *test_dev = (struct device_test *)file->private_data;
    __poll_t mask = 0;

    // Add queue to poll_table
    poll_wait(file, &read_wq, p);

    // Check if the data is ready
    if (test_dev->flag == 1) {
        mask |= POLLIN;
    }

    return mask;
}

// File operations
struct file_operations cdev_test_fops = {
    .owner = THIS_MODULE,
    .open = cdev_test_open,
    .write = cdev_test_write,
    .read = cdev_test_read,
    .release = cdev_test_release,
    .poll = cdev_test_poll,
};

// Initialize 
static int __init chr_fops_init(void)
{
    int ret;
    // Create the device number
    ret = alloc_chrdev_region(&dev1.dev_num, 0, 1, "alloc_name");
    if (ret < 0) {
        goto err_chrdev;
    }
    printk("alloc_chrdev_region is ok\r\n");

    dev1.major = MAJOR(dev1.dev_num);
    dev1.minor = MINOR(dev1.dev_num);
    printk("major is %d\r\n", dev1.major);
    printk("minor is %d\r\n", dev1.minor);

    // Initalize the cdev
    cdev_init(&dev1.cdev_test, &cdev_test_fops);
    dev1.cdev_test.owner = THIS_MODULE;

    // Add the cdev
    ret = cdev_add(&dev1.cdev_test, dev1.dev_num, 1);
    if (ret < 0) {
        goto err_chr_add;
    }

    // Create the class
    dev1.class = class_create(THIS_MODULE, "test");
    if (IS_ERR(dev1.class)) {
        ret = PTR_ERR(dev1.class);
        goto err_class_create;
    }

    // Create the device
    dev1.device = device_create(dev1.class, NULL, dev1.dev_num, NULL, "test");
    if (IS_ERR(dev1.device)) {
        ret = PTR_ERR(dev1.device);
        goto err_device_create;
    }

    return 0;

err_device_create:
    class_destroy(dev1.class);
err_class_create:
    cdev_del(&dev1.cdev_test);
err_chr_add:
    unregister_chrdev_region(dev1.dev_num, 1);
err_chrdev:
    return ret;
}

// Exit function
static void __exit chr_fops_exit(void)
{
    device_destroy(dev1.class, dev1.dev_num);
    class_destroy(dev1.class);
    cdev_del(&dev1.cdev_test);
    unregister_chrdev_region(dev1.dev_num, 1);
}

// Create a module registration macro
module_init(chr_fops_init);
module_exit(chr_fops_exit);

// Create a module declaration macro
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("Module Driver");
MODULE_AUTHOR("kongchen");
