// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/string.h>

/* ─────────── 总线:红娘(只负责 match) ─────────── */
static int mybus_match(struct device *dev, struct device_driver *drv)
{
    /* 规则:设备名和驱动名一致即配对 */
    return (strcmp(dev_name(dev), drv->name) == 0);
}

static struct bus_type mybus = {
    .name  = "mybus",
    .match = mybus_match,
    /* 注意:故意不设 .probe —— really_probe 里是 else if,
       总线有 probe 就会"挡住"驱动自己的 probe;
       留空则内核直接调 drv->probe,链路最干净 */
};

/* ─────────── 设备:公司登记(名字 = 驱动名) ─────────── */
static void mydev_release(struct device *dev)
{
    printk("This is mydev_release\n");
}

static struct device mydev = {
    .init_name = "mydriver",      /* 必须和驱动名一致,match 才能命中 */
    .bus       = &mybus,
    .release   = mydev_release,   /* 不写会在注销时报 "broken" 警告 */
};

/* ─────────── 驱动:求职者登记(probe = 面试) ─────────── */
static int mydriver_probe(struct device *dev)
{
    printk("This is mydriver_probe, 绑定的设备: %s\n", dev_name(dev));
    return 0;
}

static int mydriver_remove(struct device *dev)
{
    printk("This is mydriver_remove\n");
    return 0;
}

static struct device_driver mydriver = {
    .name   = "mydriver",
    .bus    = &mybus,
    .probe  = mydriver_probe,
    .remove = mydriver_remove,
};

/* ─────────── 注册顺序:总线 → 设备 → 驱动 ─────────── */
static int __init mydemo_init(void)
{
    int ret;

    ret = bus_register(&mybus);           /* ① /sys/bus/mybus/ 出现 */
    if (ret)
        return ret;
    printk("mydemo: bus 注册成功\n");

    ret = device_register(&mydev);        /* ② 设备注册(此刻无驱动,躺平等待) */
    if (ret)
        goto err_bus;
    printk("mydemo: device 注册成功(尚无驱动,等待匹配)\n");

    ret = driver_register(&mydriver);     /* ③ 驱动注册 → 反向遍历设备 → match 命中 → probe */
    if (ret)
        goto err_dev;
    printk("mydemo: driver 注册成功,probe 已触发,绑定完成\n");
    return 0;

err_dev:
    device_unregister(&mydev);
err_bus:
    bus_unregister(&mybus);
    return ret;
}

static void __exit mydemo_exit(void)
{
    driver_unregister(&mydriver);         /* 严格逆序拆除 */
    device_unregister(&mydev);
    bus_unregister(&mybus);
    printk("mydemo: 全部注销完成\n");
}

module_init(mydemo_init);
module_exit(mydemo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kongchen");
