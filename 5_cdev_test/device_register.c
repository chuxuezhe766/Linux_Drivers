#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>

/* Define the major parameter for the main device number when 
specifying the static loading method*/
static int major;
/* Define the minor parameter for the minor device number when
specifying the static loading method*/
static int minor;

// Pass the main device number parameter
module_param(major, int, S_IRUGO);
// Pass the minor device number parameter
module_param(minor, int, S_IRUGO);

// Define the variable dev num
static dev_t dev_num;

// Driver entry function
static int __init dev_t_init(void)
{
    // Define the variable ret
    int ret;
    /* Make a conditional judgment based on the major device number. That
    is, if the major parameter is passed in through the driver, the 
    condition is valid and it enters the following branch.*/
    if (major) {
        /* Convert the primary device number and secondary device number
        of the driver parameters to the dev_t type through the MKDEV 
        function*/ 
        dev_num = MKDEV(major, minor);
        printk("major is %d\n", major);
        printk("minor is %d\n", minor);

        // Register the device number statically
        ret = register_chrdev_region(dev_num, 1, "chrdev_name");
        if (ret) {
            printk("register_chrdev_region is error\n");
        }
        printk("register_chrdev_region is ok\n");
    }
    /* if the major parameter is not passed through the driver, 
    the condition is met and the following branch is entered.*/ 
    else {
        // Register the device number dynamically
        ret = alloc_chrdev_region(&dev_num, 0, 1, "chrdev_num");
        if (ret < 0) {
            printk("alloc_chrdev_region is error\n");
        }
        printk("alloc_chrdev_region is ok\n");
        // Obtain the MAJOR device number through the major function
        major = MAJOR(dev_num);
        // Obtain the MINOR device number through the minor function
        minor = MINOR(dev_num);
        printk("major is %d\n", major);
        printk("minor is %d\n", minor);
    } 
    return 0;
}

// Driver exit function
static void __exit dev_t_exit(void)
{
    // Cancel the character driver
    unregister_chrdev_region(dev_num, 1);
    printk("module exit\n");
}

// Create a module registration macro
module_init(dev_t_init);
module_exit(dev_t_exit);

// Create a module declaration macro
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("Module Driver");
MODULE_AUTHOR("kongchen");