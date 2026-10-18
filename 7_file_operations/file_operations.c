#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>

// chrdev open function
static int chrdev_open(struct inode *inode, struct file *file)
{
    printk("This is chrdev_open\n");
    return 0;
}

// chrdev read function
static ssize_t chrdev_read(struct file *file, char __user *buf, 
            size_t size, loff_t *off)
{
    printk("This is chrdev_read\n");
    return 0;
}

// chrdev write function
static ssize_t chrdev_write(struct file *file, const char __user *buf, 
            size_t size, loff_t *off)
{
    printk("This is chrdev_write\n");
    return 0;
}

// chrdev release function
static int chrdev_release(struct inode *inode, struct file *file)
{
    return 0;
}

// Define the variable dev num
static dev_t dev_num;
// Define the structure variable cdev test
static struct cdev cdev_test;
// Define the structure variable cdev_fops_test
static struct file_operations cdev_fops_test = {
    .owner = THIS_MODULE,
    .open = chrdev_open,
    .read = chrdev_read,
    .write = chrdev_write,
    .release = chrdev_release,
};

// Define the structure variable class_test
static struct class *class_test;

// Driver entry function
static int __init chrdev_fops_init(void)
{
    // Define the variable ret
    int ret;
    // Define the variable major, minor
    int major, minor;
    struct device *dev;
    // Automatically obtain the device number
    ret = alloc_chrdev_region(&dev_num, 0, 1, "chrdev_name");
    if (ret < 0) {
        goto err_none;
    }
    printk("alloc_chrdev_region is ok\n");

    // Obtain the major device number using MAJOR function
    major = MAJOR(dev_num);
    // Obtain the minor device number using MINOR function
    minor = MINOR(dev_num);
    printk("major is %d\n", major);
    printk("minor is %d\n", minor);

    // Initialize the cdev test structure using the cdev init function
    cdev_init(&cdev_test, &cdev_fops_test);
    cdev_test.owner = THIS_MODULE;
    
    // Add the cdev test
    ret = cdev_add(&cdev_test, dev_num, 1);
    if (ret < 0) {
        goto err_region;
    }
    printk("cdev_add is ok\n");

    // Create the class cdev
    class_test = class_create(THIS_MODULE, "class_test");
    if (IS_ERR(class_test)) {
        ret = PTR_ERR(class_test);
        goto err_add;
    }
    // Create the device
    dev = device_create(class_test, NULL, dev_num, NULL, "device_test");
    if (IS_ERR(dev)) {
        ret = PTR_ERR(dev);
        goto err_class;
    }

    return 0;

err_class:
    class_destroy(class_test);
err_add:
    cdev_del(&cdev_test);
err_region:
    unregister_chrdev_region(dev_num, 1);
err_none:
    return ret;

}

// Driver exit function
static void __exit chrdev_fops_exit(void)
{
    // Delete character device
    device_destroy(class_test, dev_num);
    // Delete class
    class_destroy(class_test);
    // Delete character device using the cdev del function
    cdev_del(&cdev_test);
    // Release the character device driver device number
    unregister_chrdev_region(dev_num, 1);

    printk("module exit\n");

}

// Create a module registration macro
module_init(chrdev_fops_init);
module_exit(chrdev_fops_exit);

// Create a module declaration macro
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("Module Driver");
MODULE_AUTHOR("kongchen");