/* 29_platform_driver/my_device.c */
#include <linux/module.h>
#include <linux/platform_device.h>
#include "myled.h"

static struct myled_pdata myled_info = {
    .gpio = 60,
    .active_low = 0,
};

static struct platform_device my_pdev = {
    .name = "myled",               /* ★ 必须和 my_driver.c 的 .driver.name 一字不差 */
    .id   = -1,                    /* 只此一个实例 */
    .dev = {.platform_data = &myled_info},
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
MODULE_AUTHOR("kongchen");