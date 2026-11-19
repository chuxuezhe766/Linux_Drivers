#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pinctrl/consumer.h>
#include <linux/slab.h>
#include <linux/of.h>

struct my_uart_data {
    struct pinctrl *pinctrl;
    struct pinctrl_state *default_state;
    struct pinctrl_state *sleep_state;
};

static void my_uart_enter_sleep(struct my_uart_data *data)
{
    if (data->sleep_state) {
        pinctrl_select_state(data->pinctrl, data->sleep_state);
    }
}

static void my_uart_enter_default(struct my_uart_data *data)
{
    pinctrl_select_state(data->pinctrl, data->default_state);
}

static int my_uart_probe(struct platform_device *pdev)
{
    struct my_uart_data *data;
    int ret;

    data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
    if (!data) {
        return -ENOMEM;
    }

    data->pinctrl = devm_pinctrl_get(&pdev->dev);
    if (IS_ERR(data->pinctrl)) {
        return PTR_ERR(data->pinctrl);
    }

    data->default_state = pinctrl_lookup_state(data->pinctrl, "default");
    if (IS_ERR(data->default_state)) {
        return PTR_ERR(data->default_state);
    }

    data->sleep_state = pinctrl_lookup_state(data->pinctrl, "sleep");
    if (IS_ERR(data->sleep_state)) {
        dev_warn(&pdev->dev, "no sleep state, keep default\n");
        data->sleep_state = NULL;
    }

    pinctrl_select_state(data->pinctrl, data->sleep_state);

    platform_set_drvdata(pdev, data);
    dev_info(&pdev->dev, "pinctrl state ready\n");
    return 0;
}

static int my_uart_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "removed\n");
    return 0;
}

static const struct of_device_id my_uart_of_match[] = {
    {.compatible = "rockchip,rk3568-uart"},
    {}
};

MODULE_DEVICE_TABLE(of, my_uart_of_match);

static struct platform_driver my_uart_driver = {
    .probe = my_uart_probe,
    .remove = my_uart_remove,
    .driver = {
        .name = "my-uart",
        .of_match_table = my_uart_of_match,
    },
};
module_platform_driver(my_uart_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kongchen");
MODULE_DESCRIPTION("pinctrl default/sleep states demo");

