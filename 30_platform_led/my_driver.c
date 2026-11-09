#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include "myled.h"

static struct myled_pdata *led_info;

static ssize_t led_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    int phys = gpio_get_value(led_info->gpio);
    return sprintf(buf, "%d\n", led_info->active_low ? !phys : !!phys);
}

static ssize_t led_store(struct device *dev, struct device_attribute *attr, 
                const char *buf, size_t count)
{
    int val;
    if (kstrtoint(buf, 0, &val)) {
        return -EINVAL;
    }

    gpio_set_value(led_info->gpio, led_info->active_low ? !val : !!val);
    return count;
}

static DEVICE_ATTR_RW(led);

static int myled_probe(struct platform_device *pdev)
{
    int ret;

    led_info = dev_get_platdata(&pdev->dev);
    if (!led_info) {
        dev_err(&pdev->dev, "缺少 platform_data\n");
        return -EINVAL;
    }
    dev_info(&pdev->dev, "probe: gpio=%d\n", led_info->gpio);

    ret = devm_gpio_request(&pdev->dev, led_info->gpio, "myled");
    if (ret) {
        dev_err(&pdev->dev, "GPIO %d 申请失败:%d(可能被占用)\n", led_info->gpio, ret);
        return ret;
    }

    gpio_direction_output(led_info->gpio, led_info->active_low ? 1 : 0);

    platform_set_drvdata(pdev, led_info);
    ret = device_create_file(&pdev->dev, &dev_attr_led);
    if (ret) {
        return ret;
    }
    return 0;
}

static int myled_remove(struct platform_device *pdev)
{
    device_remove_file(&pdev->dev, &dev_attr_led);
    gpio_set_value(led_info->gpio, led_info->active_low ? 1 : 0);
    return 0;
}

static struct platform_driver myled_driver = {
    .probe = myled_probe,
    .remove = myled_remove,
    .driver = {.name = "myled"},
};

module_platform_driver(myled_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kongchen");