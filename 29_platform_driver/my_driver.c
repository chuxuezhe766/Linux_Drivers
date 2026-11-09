#include <linux/module.h>             /* module_platform_driver 里的 module_* 宏 */
#include <linux/platform_device.h>

static int mydrv_probe(struct platform_device *pdev)
{
    pr_info("mydrv probe: matched with %s\n", pdev->name);
    return 0;
}

static int mydrv_remove(struct platform_device *pdev)
{
    pr_info("mydrv remove: %s\n", pdev->name);
    return 0;
}

static struct platform_driver my_pdrv = {
    .probe = mydrv_probe,
    .remove = mydrv_remove,
    .driver = {
        .name = "my_platform_dev",
    },
};

static int __init mydrv_init(void)
{
    return platform_driver_register(&my_pdrv);
}

static void __exit mydrv_exit(void)
{
    platform_driver_unregister(&my_pdrv);
}

module_init(mydrv_init);
module_exit(mydrv_exit);

MODULE_LICENSE("GPL");