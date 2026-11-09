#include <linux/module.h>             /* module_platform_driver 里的 module_* 宏 */
#include <linux/platform_device.h>

// 设备端(my_device.c)
static struct platform_device my_device = {
    .name = "my_device",
    .id = -1,
};
static int __init my_device_init(void)
{
    return platform_device_register(&my_device);
}
static void __exit my_device_exit(void)
{
    platform_device_unregister(&my_device);
}

module_init(my_device_init);
module_exit(my_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kongchen");
