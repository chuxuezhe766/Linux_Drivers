#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/gpio/consumer.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/delay.h>    /* udelay */

// The structure of private data
struct my_periph_dev {
    struct device *dev;
    void __iomem *base;
    int irq;
    struct gpio_desc *enable_gpio;
    struct gpio_desc *reset_gpio;
    struct clk *core_clk;
    struct clk *pclk;

    u32 frequency;
    u32 buffer_size;
    const char *device_name;
    u32 multi_value[8];
    int multi_count;
    bool fast_mode;
};

// Irq function
static irqreturn_t my_periph_irq_handler(int irq, void *dev_id)
{
    struct my_periph_dev *mpdev = dev_id;
    dev_info(mpdev->dev, "中断触发！ IRQ = %d\n", irq);
    return IRQ_HANDLED;
}

// Probe function
static int my_periph_probe(struct platform_device *pdev)
{
    struct my_periph_dev *mpdev;
    struct device_node *np = pdev->dev.of_node;
    struct resource *res;
    int ret;

    // Allocate private data
    mpdev = devm_kzalloc(&pdev->dev, sizeof(*mpdev), GFP_KERNEL);
    if (!mpdev) {
        return -ENOMEM;
    }
    mpdev->dev = &pdev->dev;
    platform_set_drvdata(pdev, mpdev);
    dev_info(&pdev->dev, "=== 设备树综合实验开始 ===\n");

    // Register resource acquisition
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        dev_err(&pdev->dev, "获取寄存器资源失败\n");
        return -ENODEV;
    }
    dev_info(&pdev->dev, "寄存器资源：%pR\n", res);

    mpdev->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(mpdev->base)) {
        return PTR_ERR(mpdev->base);
    }
    dev_info(&pdev->dev, "寄存器映射地址: %p\n", mpdev->base);

    // Interrupt acquisition
    mpdev->irq = platform_get_irq(pdev, 0);
    if (mpdev->irq < 0) {
        dev_err(&pdev->dev, "获取中断失败：%d\n", mpdev->irq);
        return mpdev->irq;
    }
    dev_info(&pdev->dev, "中断号：%d\n", mpdev->irq);

    // Register the interrupt handling function
    ret = devm_request_irq(&pdev->dev, mpdev->irq, my_periph_irq_handler,
                IRQF_TRIGGER_HIGH, "my_periph", mpdev);
    if (ret) {
        dev_err(&pdev->dev, "注册中断失败：%d\n", ret);
        return ret;
    }

    // GPIO acquisiton
    mpdev->enable_gpio = devm_gpiod_get(&pdev->dev, "enable", GPIOD_OUT_HIGH);
    if (IS_ERR(mpdev->enable_gpio)) {
        dev_err(&pdev->dev, "获取 enable GPIO 失败: %ld\n",
                PTR_ERR(mpdev->enable_gpio));
        return PTR_ERR(mpdev->enable_gpio);
    }
    dev_info(&pdev->dev, "enable GPIO 已获取，输出高电平\n");

    mpdev->reset_gpio = devm_gpiod_get(&pdev->dev, "reset", GPIOD_OUT_HIGH);
    if (IS_ERR(mpdev->reset_gpio)) {
        dev_err(&pdev->dev, "获取 reset GPIO 失败: %ld\n",
                PTR_ERR(mpdev->reset_gpio));
        return PTR_ERR(mpdev->reset_gpio);
    }

    // Clock acquisition
    mpdev->core_clk = devm_clk_get(&pdev->dev, "core_clk");
    if (IS_ERR(mpdev->core_clk)) {
        dev_err(&pdev->dev, "获取 core_clk 失败：%ld\n", 
            PTR_ERR(mpdev->core_clk));
        return PTR_ERR(mpdev->core_clk);
    }

    mpdev->pclk = devm_clk_get(&pdev->dev, "pclk");
    if (IS_ERR(mpdev->pclk)) {
        dev_err(&pdev->dev, "获取 pclk 失败：%ld\n", PTR_ERR(mpdev->pclk));
        return PTR_ERR(mpdev->pclk);
    }

    ret = clk_prepare_enable(mpdev->core_clk);
    if (ret) {
        clk_disable_unprepare(mpdev->core_clk);
        return ret;
    }
    ret = clk_prepare_enable(mpdev->pclk);
    if (ret) {
        clk_disable_unprepare(mpdev->core_clk);
        return ret;
    }

    dev_info(&pdev->dev, "core_clk 频率：%lu Hz\n",
            clk_get_rate(mpdev->core_clk));
    dev_info(&pdev->dev, "pclk 频率：%lu Hz\n",
            clk_get_rate(mpdev->pclk));

    // u32 property
    ret = of_property_read_u32(np, "my,frequency", &mpdev->frequency);
    if (ret) {
        dev_warn(&pdev->dev, "读取my,frequency 失败\n");
    }
    else {
        dev_info(&pdev->dev, "my,frequency = %u\n", mpdev->frequency);
    }
    ret = of_property_read_u32(np, "my,buffer-size", &mpdev->buffer_size);
    if (!ret) {
        dev_info(&pdev->dev, "my,buffer-size = %u\n", mpdev->buffer_size);
    }

    // u32 multi_property
    mpdev->multi_count = of_property_count_u32_elems(np, "my,multi-value");
    if (mpdev->multi_count > 0 && mpdev->multi_count <=8) {
        of_property_read_u32_array(np, "my,multi-value", 
                mpdev->multi_value, mpdev->multi_count);
        for (ret = 0; ret < mpdev->multi_count; ret++) {
            dev_info(&pdev->dev, "my,multi-value[%d] = 0x%02x\n",
                    ret, mpdev->multi_value[ret]);
        }
    }

    // bool property
    mpdev->fast_mode = of_property_read_bool(np, "my,fast-mode");
    dev_info(&pdev->dev, "my,fast-mode = %s\n",
            mpdev->fast_mode ? "true" : "false");
    
    // other node informations
    dev_info(&pdev->dev, "节点名：%s\n", np->name);
    dev_info(&pdev->dev, "完整路径：%s\n", np->full_name);

    {
        struct device_node *parent = of_get_parent(np);
        if (parent) {
            dev_info(&pdev->dev, "父节点：%s\n", parent->full_name);
            of_node_put(parent);
        }
    }

    dev_info(&pdev->dev, "=== 设备树综合实验完成 ===\n");
    return 0;
}

// Remove function
static int my_periph_remove(struct platform_device *pdev)
{
    struct my_periph_dev *mpdev = platform_get_drvdata(pdev);

    clk_disable_unprepare(mpdev->pclk);
    clk_disable_unprepare(mpdev->core_clk);

    dev_info(&pdev->dev, "设备移除\n");
    return 0;
}


// Match
static const struct of_device_id my_periph_of_match[] = {
    {.compatible = "my,periph-demo"},
    {}
};
MODULE_DEVICE_TABLE(of, my_periph_of_match);

// Platform driver
static struct platform_driver my_periph_driver = {
    .probe = my_periph_probe,
    .remove = my_periph_remove,
    .driver = {
        .name = "my-periph",
        .of_match_table = my_periph_of_match,
    },
};
module_platform_driver(my_periph_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RK3568 Demo");
MODULE_DESCRIPTION("设备树综合实验驱动");