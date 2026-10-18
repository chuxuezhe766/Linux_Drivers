#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

// Define the variable dev num
static dev_t dev_num;
// Define the variable major
static int major = 0;
// Define the variable minor
static int minor = 0;
// Define the variable cdev test
struct cdev cdev_test;
// Define the variable class
struct class *class;
// Define the variable device
struct device *device;

// Open function
static int cdev_test_open(struct inode *inode, struct file *file)
{
    printk("This is cdev_test_open\n");
    return 0;
}

// Write function
static ssize_t cdev_test_write(struct file *file, const char __user *buf, size_t size, loff_t *off)
{
    // Define the variable kbuf
    char kbuf[32] = "This is cdev_test_read!";
    if (copy_from_user(kbuf, buf, size) != 0) {
        printk("copy_from_user error\r\n");
        return -1;
    }
    printk("This is cdev_test_write\r\n");
    printk("kbuf is %s\r\n", kbuf);
    return 0;
}

// Read function
static ssize_t cdev_test_read(struct file *file, char __user *buf, size_t size, loff_t *off)
{
    // Define the variable kbuf
    char kbuf[32] = "This is cdev_test_read!";
    if (copy_to_user(buf, kbuf, strlen(kbuf)) != 0) {
        printk("copy_to_user error\r\n");
        return -1;
    }

    printk("This is cdev_test_read\r\n");
    return 0;
}

// Release function
static int cdev_test_release(struct inode *inode, struct file *file)
{
    printk("This is cdev_test_release\r\n");
    return 0;
}

// Define the variable cdev_test_fops
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
    // Define the variable ret
    int ret;
    // Create the device number 
    ret = alloc_chrdev_region(&dev_num, 0, 1, "alloc_name");
    if (ret < 0) {
        printk("alloc_chrdev_region is error\n");
        goto err_none;
    }
    printk("alloc_chrdev_region is ok\n");
    // Obtain the major device number using MAJOR function
    major = MAJOR(dev_num);
    // Obtain the minor device number using MINOR function
    minor = MINOR(dev_num);

    printk("major is %d\r\n", major);
    printk("minor is %d\r\n", minor);

    // Initialize the cdev test structure using the cdev init function
    cdev_init(&cdev_test, &cdev_test_fops);
    cdev_test.owner = THIS_MODULE;
    // Add teh cdev test
    ret = cdev_add(&cdev_test, dev_num, 1);
    if (ret < 0) {
        printk("cdev_add is error\n");
        goto err_region;
    }
    // Create the class
    class = class_create(THIS_MODULE, "test");
    if (IS_ERR(class)) {
        ret = PTR_ERR(class);
        goto err_add;
    }
    // Create the device
    device = device_create(class, NULL, dev_num, NULL, "test");
    if (IS_ERR(device)) {
        ret = PTR_ERR(device);
        goto err_class;
    }

    return 0;

err_class:
    class_destroy(class);
err_add:
    cdev_del(&cdev_test);
err_region:
    unregister_chrdev_region(dev_num, 1);
err_none:
    return ret;
}

// Driver exit function
static void __exit chr_fops_exit(void)
{
    unregister_chrdev_region(dev_num, 1);
    cdev_del(&cdev_test);
    device_destroy(class, dev_num);
    class_destroy(class);
}

// Create a module registration macro
module_init(chr_fops_init);
module_exit(chr_fops_exit);

// Create a module declaration macro
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("Module Driver");
MODULE_AUTHOR("kongchen");