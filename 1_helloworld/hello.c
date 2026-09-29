#include <linux/module.h>
#include <linux/kernel.h>

// 驱动入口函数
static int __init helloworld_init(void)
{
    printk("helloworld_init\r\n");// 内核打印
    return 0;
}

// 驱动出口函数
static void __exit helloworld_exit(void)
{
    printk("helloworld_exit\r\n");
}

// 声明模块入口函数
module_init(helloworld_init);
// 声明模块出口函数
module_exit(helloworld_exit);

// 开源协议
MODULE_LICENSE("GPL v2");
// 驱动版本
MODULE_VERSION("1.0");
// 驱动信息
MODULE_DESCRIPTION("helloworld Driver");
// 驱动作者
MODULE_AUTHOR("kongchen");