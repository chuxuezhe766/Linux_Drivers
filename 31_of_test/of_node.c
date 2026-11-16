#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/of.h>

static int __init of_node_demo_init(void)
{
    struct device_node *np;

    /* 方式1：通过 compatible 查找（驱动最常用） */
    np = of_find_compatible_node(NULL, NULL, "my,custom-node");
    if (np) {
        pr_info("方式1：通过 compatible 找到节点，name: %s, full_name: %s\n",
                np->name, np->full_name);
        of_node_put(np);  /* 查找后必须释放引用计数 */
    }

    /* 方式2：通过绝对路径查找（最精准） */
    np = of_find_node_by_path("/my_custom_node");
    if (np) {
        pr_info("方式2：通过路径找到节点，name: %s\n", np->name);
        of_node_put(np);
    }

    /* 方式3：通过节点名查找 */
    np = of_find_node_by_name(NULL, "my_custom_node");
    if (np) {
        pr_info("方式3：通过节点名找到节点，full_name: %s\n", np->full_name);
        of_node_put(np);
    }

    /* 方式4：遍历子节点 */
    {
        struct device_node *root, *child;
        root = of_find_node_by_path("/");
        if (root) {
            int count = 0;
            for_each_child_of_node(root, child) {
                count++;
            }
            pr_info("方式4：根节点下共有 %d 个子节点\n", count);
        }
    }

    /* 方式5：获取父节点 */
    np = of_find_compatible_node(NULL, NULL, "my,custom-node");
    if (np) {
        struct device_node *parent = of_get_parent(np);
        if (parent) {
            pr_info("方式5：my_custom_node 的父节点: %s\n", parent->full_name);
            of_node_put(parent);
        }
        of_node_put(np);
    }

    return 0;
}

static void __exit of_node_demo_exit(void)
{
    pr_info("OF 节点查找实验驱动退出\n");
}

module_init(of_node_demo_init);
module_exit(of_node_demo_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RK3568 OF Node Lookup Demo");