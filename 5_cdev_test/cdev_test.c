#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>

// Define the variable dev_num
static dev_t dev_num;
// Define the structure variable cdev_test;
static struct cdev cdev_test;

// Define the file_operation structure variable cdev_test_ops
static struct file_operations cdev_test_ops = {
    // Avoid uninstalling the module when it is in use
    .owner = THIS_MODULE,
};

// Driver entry function
static int __init module_cdev_init(void)
{
    // Define the variable ret
    int ret;
    // Define the variable major, minor
    int major, minor;
    // Automatically obtain the device number
    ret = alloc_chrdev_region(&dev_num, 0, 1, "chrdev_name");
    if (ret < 0) {
        printk("alloc_chrdev_region is error\n");
    }
    printk("alloc_register_region is ok\n");
    // Obtain the MAJOR device number through the major function
    major = MAJOR(dev_num);
    // Obtain the MINOR device number through the minor function
    minor = MINOR(dev_num);
    printk("major is %d\n", major);
    printk("minor is %d\n", minor);
    // Initialize the cdev test structure using the cdev init funciton
    cdev_init(&cdev_test, &cdev_test_ops);
    cdev_test.owner = THIS_MODULE;
    // Add the cdev test
    ret = cdev_add(&cdev_test, dev_num, 1);
    if (ret < 0) {
        printk("cdev_add is error\n");
    }
    printk("cdev_add is ok\n");
    return 0;
}

// Driver exit function
static void __exit module_cdev_exit(void)
{
    // Delete character device using the cdev del function
    cdev_del(&cdev_test);
    // Release the character device driver device number
    unregister_chrdev_region(dev_num, 1);
    printk("module exit\n");
}

// Create a module registration macro
module_init(module_cdev_init);
module_exit(module_cdev_exit);

// Create a module declaration macro
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("Module Driver");
MODULE_AUTHOR("kongchen");