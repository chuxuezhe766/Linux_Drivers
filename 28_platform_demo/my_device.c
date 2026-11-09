#include <linux/module.h>             /* module_platform_driver 里的 module_* 宏 */
#include <linux/platform_device.h>    /* platform_driver、module_platform_driver(:259)、to_platform_device */

static int my_probe(struct platform_device *pdev)
{
    printk(KERN_INFO "Probe called!\n");
    return 0;
}
static struct platform_driver my_driver = {
    .driver = {
        .name = "my_device",
    },
    .probe = my_probe,
};
module_platform_driver(my_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kongchen");
