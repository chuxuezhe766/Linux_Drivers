/* 设备树中断解析实验驱动 */
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>

static int irq_demo_probe(struct platform_device *pdev)
{
    struct device_node *np = pdev->dev.of_node;
    int irq, i;
    int virq;   /* 必须是 int：of_irq_get 失败时返回负数 */

    /* 方式1：platform_get_irq（驱动标准方式） */
    irq = platform_get_irq(pdev, 0);
    if (irq > 0)
        dev_info(&pdev->dev, "platform_get_irq: IRQ = %d\n", irq);

    /* 方式2：irq_of_parse_and_map（直接 OF 解析） */
    virq = irq_of_parse_and_map(np, 0);
    if (virq)
        dev_info(&pdev->dev, "irq_of_parse_and_map: virq = %u\n", virq);

    /* 方式3：of_irq_get */
    virq = of_irq_get(np, 0);
    if (virq > 0)
        dev_info(&pdev->dev, "of_irq_get: virq = %d\n", virq);

    /* 统计节点的中断数量：of_irq_count 未导出，模块里用 platform_irq_count */
    i = platform_irq_count(pdev);
    dev_info(&pdev->dev, "此节点共有 %d 个中断\n", i);

    return 0;
}

static const struct of_device_id irq_demo_match[] = {
    { .compatible = "test,irq-demo" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, irq_demo_match);

static struct platform_driver irq_demo_driver = {
    .probe = irq_demo_probe,
    .driver = {
        .name = "irq_demo",
        .of_match_table = irq_demo_match,
    },
};
module_platform_driver(irq_demo_driver);
MODULE_LICENSE("GPL");