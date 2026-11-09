#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/configfs.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/device.h>

int mybus_match(struct device *dev, struct device_driver *drv)
{
    return (strcmp(dev_name(dev), drv->name) == 0);
}

int mybus_probe(struct device *dev)
{
    struct device_driver *drv = dev->driver;
    if (drv->probe) {
        drv->probe(dev);
    }
    return 0;
}

struct bus_type mybus = {
    .name = "mybus",
    .match = mybus_match,
    .probe = mybus_probe,
};

static int bus_init(void)
{
    int ret;
    ret = bus_register(&mybus);
    return 0;
}

static void bus_exit(void)
{
    bus_unregister(&mybus);
}


module_init(bus_init);
module_exit(bus_exit);

MODULE_LICENSE("GPL");
