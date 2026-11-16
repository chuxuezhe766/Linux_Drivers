#include <linux/module.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/of.h>    /* struct of_device_id 定义在 mod_devicetable.h，of.h 会带上 */

struct led_data {
    struct gpio_desc *led_gpio;
};

static int led_probe(struct platform_device *pdev)
{
    struct led_data *data;

    data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    /* 从设备树获取 led-gpios，初始化为输出低电平 */
    data->led_gpio = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_HIGH);
    if (IS_ERR(data->led_gpio)) {
        dev_err(&pdev->dev, "Failed to get GPIO\n");
        return PTR_ERR(data->led_gpio);
    }

    dev_info(&pdev->dev, "LED initialized\n");
    return 0;
}

static int led_remove(struct platform_device *pdev)
{
    /* devm_ 系列自动释放，无需手动 gpiod_put */
    dev_info(&pdev->dev, "LED removed\n");
    return 0;
}

static const struct of_device_id led_of_ids[] = {
    { .compatible = "my,led" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, led_of_ids);

static struct platform_driver led_driver = {
    .driver = {
        .name = "my-led",
        .of_match_table = led_of_ids,
    },
    .probe = led_probe,
    .remove = led_remove,
};
module_platform_driver(led_driver);
MODULE_LICENSE("GPL");
