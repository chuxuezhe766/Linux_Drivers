#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>

// Define the variable dev_num
static dev_t dev_num;
// Define the structure variable cdev_test
static struct cdev cdev_test;
// Define the file_operation structure variable cdev_test_ops
static struct file_operations cdev_fops_test = {
    // Avoid uninstalling the module when it is in use
    .owner = THIS_MODULE,
};
// Define the structure variable class_test
static struct class *class_test;

// Driver entry function
static int __init chrdev_fops_init(void)
{
    // Define the variable ret
    int ret;
    // Define the variable major,minor
    int major, minor;
    // Automatically obtain the device number
    struct device *dev;
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
    // Initialize the cdev test structure using the cdev init funciton
    cdev_init(&cdev_test, &cdev_fops_test);
    cdev_test.owner = THIS_MODULE;
    // Add the cdev test
    ret = cdev_add(&cdev_test, dev_num, 1);
    if (ret < 0) {
        goto err_region;
    }
    printk("cdev_add is ok\n");
    class_test = class_create(THIS_MODULE, "class_test");
    if (IS_ERR(class_test)) {
        ret = PTR_ERR(class_test);
        goto err_add;
    }

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
    device_destroy(class_test, dev_num);
    // Delete class
    class_destroy(class_test);
    // Delete character device using the cdev del function
    cdev_del(&cdev_test);
    // Release the character device driver device number
    unregister_chrdev_region(dev_num, 1);
    // Delete character device 
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