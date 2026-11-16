#include <linux/module.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/of.h> 

struct button_data {
    struct gpio_desc *button_gpio;
    int irq;
};

static irqreturn_t button_irq_handler(int irq, void *dev_id)
{
    struct button_data *data = dev_id;
    int value = gpiod_get_value(data->button_gpio);
    pr_info("Button IRQ! value = %d\n", value);
    return IRQ_HANDLED;
}

static int button_probe(struct platform_device *pdev)
{
    struct button_data *data;
    int ret;

    data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    /* 获取 button-gpios，设为输入 */
    data->button_gpio = devm_gpiod_get(&pdev->dev, "button", GPIOD_IN);
    if (IS_ERR(data->button_gpio))
        return PTR_ERR(data->button_gpio);

    /* GPIO 转中断号 */
    data->irq = gpiod_to_irq(data->button_gpio);
    if (data->irq < 0)
        return data->irq;

    /* 注册中断（下降沿触发） */
    ret = devm_request_irq(&pdev->dev, data->irq,
                           button_irq_handler,
                           IRQF_TRIGGER_FALLING,
                           "button", data);
    if (ret) {
        dev_err(&pdev->dev, "Failed to request IRQ\n");
        return ret;
    }

    dev_info(&pdev->dev, "Button initialized, irq=%d\n", data->irq);
    return 0;
}

static const struct of_device_id button_of_ids[] = {
    { .compatible = "my,button" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, button_of_ids);

static struct platform_driver button_driver = {
    .driver = {
        .name = "my-button",
        .of_match_table = button_of_ids,
    },
    .probe = button_probe,
};
module_platform_driver(button_driver);
MODULE_LICENSE("GPL");