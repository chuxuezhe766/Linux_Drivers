#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

// 魔术数和命令定义
#define IOCTL_MAGIC 'M'
#define IOCTL_RESET         _IO(IOCTL_MAGIC, 0)
#define IOCTL_GET_VER       _IOR(IOCTL_MAGIC, 1, int)
#define IOCTL_SET_MODE      _IOW(IOCTL_MAGIC, 2, int)
#define IOCTL_GET_STATUS    _IOR(IOCTL_MAGIC, 3, int)
#define IOCTL_SET_DATA      _IOW(IOCTL_MAGIC, 4, struct ioctl_data)
#define IOCTL_GET_DATA      _IOR(IOCTL_MAGIC, 5, struct ioctl_data)

// the variable ioctl data
struct ioctl_data {
    char name[32];
    int value;
};

// the status of device
static struct device_state {
    int version;
    int mode;
    int status;
    struct ioctl_data data;
};

static struct device_state device_status = {
    .version = 100,
    .mode = 0,
    .status = 1,
};

// open
static int ioctl_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "ioctl device opened\n");
    return 0;
}

// release
static int ioctl_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "ioctl device closed\n");
    return 0;
}                                

// unlocked_ioctl
static long ioctl_cmd(struct file *file, unsigned int cmd,
                        unsigned long arg)
{
    int ret = 0;
    int value;
    struct ioctl_data user_data;

    switch (cmd) {
        case IOCTL_RESET:
            // Reset device
            printk(KERN_INFO "Resetting device...\n");
            device_status.mode = 0;
            device_status.status = 1;
            memset(&device_status.data, 0, sizeof(device_status.data));
            break;
        case IOCTL_GET_VER:
            // Obtain the device number
            value = device_status.version;
            if (copy_to_user((int __user *)arg, &value, sizeof(value))) {
                return -EFAULT;
            }
            printk(KERN_INFO "Get version: %d\n", value);
            break;
        case IOCTL_SET_MODE:
            // Set mode 
            if (copy_from_user(&value, (int __user *)arg, sizeof(value))) {
                return -EFAULT;
            }

            if (value < 0 || value > 3) {
                printk(KERN_ERR "Invalid mode: %d\n", value);
                return -EINVAL;
            }

            device_status.mode = value;
            printk(KERN_INFO "Set mode to: %d\n", value);
            break;
        case IOCTL_GET_STATUS:
            // Obtain the device status
            value = device_status.status;
            if (copy_to_user((int __user *)arg, &value, sizeof(value))) {
                return -EFAULT;
            }
            break;
        case IOCTL_SET_DATA:
            // Set data
            if (copy_from_user(&user_data, (struct ioctl_data __user *)arg, 
                    sizeof(user_data))) {
                return -EFAULT;
            }
            memcpy(&device_status.data, &user_data, sizeof(user_data));
            printk(KERN_INFO "Set data: name=%s, value=%d\n", user_data.name, user_data.value);
            break;
        case IOCTL_GET_DATA:
            // Obtain data
            if (copy_to_user((struct ioctl_data __user *)arg, &device_status.data,
                    sizeof(device_status.data))) {
                return -EFAULT;
            }
            break;
        default:
            printk(KERN_WARNING "Unknown ioctl cmd: 0x%x\n", cmd);
            return -EINVAL;
        
    }

    return ret;
}

// file_operations
static struct file_operations ioctl_fops = {
    .owner = THIS_MODULE,
    .open = ioctl_open,
    .release = ioctl_release,
    .unlocked_ioctl = ioctl_cmd,
};

// the gobal variable
static dev_t dev_num;
static struct cdev ioctl_cdev;
static struct class *ioctl_class;

// Initialize
static int __init ioctl_init(void)
{
    int ret;
    printk(KERN_INFO "=== IOCTL Demo ===\n");
    ret = alloc_chrdev_region(&dev_num, 0, 1, "ioctl_demo");
    if (ret < 0) {
        return ret;
    }

    cdev_init(&ioctl_cdev, &ioctl_fops);
    ioctl_cdev.owner = THIS_MODULE;

    ret = cdev_add(&ioctl_cdev, dev_num, 1);
    if (ret < 0) {
        goto err_cdev;
    }

    ioctl_class = class_create(THIS_MODULE, "ioctl_class");
    if (IS_ERR(ioctl_class)) {
        ret = PTR_ERR(ioctl_class);
        goto err_class;
    }

    device_create(ioctl_class, NULL, dev_num, NULL, "ioctl_test");

    printk(KERN_INFO "IOCTL demo loaded\n");
    return 0;

err_class:
    cdev_del(&ioctl_cdev);
err_cdev:
    unregister_chrdev_region(dev_num, 1);
    return ret;
}

// exit function
static void __exit ioctl_exit(void)
{
    device_destroy(ioctl_class, dev_num);
    class_destroy(ioctl_class);
    cdev_del(&ioctl_cdev);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "IOCTL demo unloaded");
}

// Create a module registration macro
module_init(ioctl_init);
module_exit(ioctl_exit);

// Create a module declaration macro
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("Module Driver");
MODULE_AUTHOR("kongchen");
