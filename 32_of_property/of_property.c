#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>

static int of_parse_demo_probe(struct platform_device *pdev)
{
    struct device_node *np = pdev->dev.of_node;
    struct property *prop;
    u32 val_u32, arr[10];
    u64 val_u64;
    const char *str, *str_arr[10];
    int ret, cnt, i;

    /* 1. of_find_property：查找属性，获取原始 property 结构 */
    prop = of_find_property(np, "data-array", NULL);
    if (prop)
        pr_info("1. of_find_property: found \"%s\", length=%d bytes\n",
               prop->name, prop->length);

    /* 2. of_property_count_elems_of_size：统计 u32 元素个数 */
    cnt = of_property_count_elems_of_size(np, "data-array", sizeof(u32));
    pr_info("2. data-array has %d u32 elements\n", cnt);

    /* 3. of_property_read_u32_index：按索引读取 u32 */
    ret = of_property_read_u32_index(np, "data-array", 2, &val_u32);
    if (!ret)
        pr_info("3. data-array[2] = 0x%08x\n", val_u32);

    /* 4. of_property_read_u64：读取 64 位值 */
    ret = of_property_read_u64(np, "single-64bit", &val_u64);
    if (!ret)
        pr_info("4. single-64bit = 0x%016llx\n", val_u64);

    /* 5. of_property_read_variable_u32_array：读取 u32 数组 */
    ret = of_property_read_variable_u32_array(np, "data-array", arr, 1, 10);
    if (ret > 0) {
        pr_info("5. read %d values from data-array:\n", ret);
        for (i = 0; i < ret; i++)
            pr_info("   arr[%d] = 0x%08x\n", i, arr[i]);
    }

    /* 6. of_property_read_string：读取字符串 */
    ret = of_property_read_string(np, "device-name", &str);
    if (!ret)
        pr_info("6. device-name = %s\n", str);

    /* 7. of_property_read_string_array：读取字符串数组 */
    ret = of_property_read_string_array(np, "status-list", str_arr, 3);
    if (ret > 0) {
        pr_info("7. status-list:\n");
        for (i = 0; i < ret; i++)
            pr_info("   [%d] = %s\n", i, str_arr[i]);
    }

    /* 8. of_property_read_bool：判断布尔属性 */
    if (of_property_read_bool(np, "power-enable"))
        pr_info("8. power-enable = true\n");

    /* 9. 解析 reg 属性 */
    ret = of_property_read_variable_u32_array(np, "reg", arr, 2, 10);
    if (ret > 0) {
        pr_info("9. reg (addr + size pairs):\n");
        for (i = 0; i < ret; i += 2)
            pr_info("   addr: 0x%08x, size: 0x%08x\n", arr[i], arr[i+1]);
    }

    return 0;
}

static const struct of_device_id of_parse_demo_match[] = {
    { .compatible = "test,of-demo" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, of_parse_demo_match);

static struct platform_driver of_parse_demo_driver = {
    .probe = of_parse_demo_probe,
    .driver = {
        .name = "of_parse_demo",
        .of_match_table = of_parse_demo_match,
    },
};
module_platform_driver(of_parse_demo_driver);
MODULE_LICENSE("GPL");