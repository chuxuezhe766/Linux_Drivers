/* 设备树资源解析实验驱动 */
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/io.h>

static int res_demo_probe(struct platform_device *pdev)
{
    struct device_node *np = pdev->dev.of_node;
    struct resource *res;
    void __iomem *base;
    int i;

    /* 方式1：of_address_to_resource 手动转换 */
    {
        struct resource r;
        if (of_address_to_resource(np, 0, &r) == 0)
            dev_info(&pdev->dev,
                     "of_address_to_resource: start=0x%llx end=0x%llx flags=0x%lx\n",
                     (u64)r.start, (u64)r.end, r.flags);
    }

    /* 方式2：platform_get_resource（驱动标准方式） */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (res)
        dev_info(&pdev->dev, "platform_get_resource: %pR\n", res);

    /* 方式3：devm_ioremap_resource（申请 + 映射一步完成） */
    base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(base))
        return PTR_ERR(base);
    dev_info(&pdev->dev, "ioremap: base=%p\n", base);

    /* 方式4：of_iomap（直接映射，不申请资源） */
    base = of_iomap(np, 0);
    if (base)
        dev_info(&pdev->dev, "of_iomap: base=%p\n", base);

    /* 遍历所有 reg 资源 */
    for (i = 0; ; i++) {
        res = platform_get_resource(pdev, IORESOURCE_MEM, i);
        if (!res)
            break;
        dev_info(&pdev->dev, "reg[%d]: %pR\n", i, res);
    }

    return 0;
}

static const struct of_device_id res_demo_match[] = {
    { .compatible = "test,res-demo" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, res_demo_match);

static struct platform_driver res_demo_driver = {
    .probe = res_demo_probe,
    .driver = {
        .name = "res_demo",
        .of_match_table = res_demo_match,
    },
};
module_platform_driver(res_demo_driver);
MODULE_LICENSE("GPL");