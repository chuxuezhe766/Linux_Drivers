/* 29_platform_driver/my_device.c */
#include <linux/module.h>
#include <linux/platform_device.h>

static struct platform_device my_pdev = {
    .name = "my_platform_dev",     /* ★ 必须和 my_driver.c 的 .driver.name 一字不差 */
    .id   = -1,                    /* 只此一个实例 */
};

static int __init my_pdev_init(void)
{
    return platform_device_register(&my_pdev);
}
static void __exit my_pdev_exit(void)
{
    platform_device_unregister(&my_pdev);
}
module_init(my_pdev_init);
module_exit(my_pdev_exit);
MODULE_LICENSE("GPL");