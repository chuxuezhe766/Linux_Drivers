/* RK3506 UART 驱动框架 */
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>

/* 匹配表：一个驱动支持多种设备 */
static const struct of_device_id my_uart_of_match[] = {
    {
        .compatible = "rockchip,rk3506-uart",  /* RK3506 专用 */
        .data = (void *)0x1234,                 /* 自定义数据 */
    },
    {
        .compatible = "haoyu,hym8563",       /* DesignWare 通用 */
        .data = (void *)0x5678,
    },
    { /* sentinel */ }   /* 数组结束标记，必须有 */
};
MODULE_DEVICE_TABLE(of, my_uart_of_match);

static int my_uart_probe(struct platform_device *pdev)
{
    struct device_node *np = pdev->dev.of_node;
    const struct of_device_id *match;
    const void *data;
    u32 port_type;

    /* 方式1：通过 match 指针获取匹配信息 */
    match = of_match_device(my_uart_of_match, &pdev->dev);
    if (match) {
        dev_info(&pdev->dev, "匹配成功: compatible=%s\n", match->compatible);
        port_type = (u32)(unsigned long)match->data;
    }

    /* 方式2：直接获取 data（更简洁） */
    data = of_device_get_match_data(&pdev->dev);

    /* 方式3：手动读取 compatible */
    {
        const char *compat;
        if (of_property_read_string_index(np, "compatible", 0, &compat) == 0)
            dev_info(&pdev->dev, "compatible[0] = %s\n", compat);
    }

    /* 获取资源 */
    {
        struct resource *res;
        void __iomem *base;
        int irq;

        res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
        base = devm_ioremap_resource(&pdev->dev, res);

        irq = platform_get_irq(pdev, 0);
    }

    return 0;
}

static struct platform_driver my_uart_driver = {
    .probe = my_uart_probe,
    .driver = {
        .name = "my-uart",
        .of_match_table = my_uart_of_match,  /* 关键：绑定匹配表 */
    },
};
module_platform_driver(my_uart_driver);
MODULE_LICENSE("GPL");