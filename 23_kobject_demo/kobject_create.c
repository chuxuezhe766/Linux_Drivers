// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

/* ───────────── 演示数据 ───────────── */
static int demo_value = 100;

/* ───────────── 属性文件:sysfs 的 cat/echo 会调用它们 ───────────── */
static ssize_t value_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", demo_value);
}

static ssize_t value_store(struct kobject *kobj, struct kobj_attribute *attr,
                           const char *buf, size_t count)
{
    int val, ret;

    ret = kstrtoint(buf, 0, &val);
    if (ret)
        return ret;

    demo_value = val;
    printk(KERN_INFO "[kobj_demo] value 被设置为 %d\n", demo_value);
    return count;
}
static struct kobj_attribute value_attr = __ATTR(value, 0644, value_show, value_store);

static struct attribute *demo_attrs[] = {
    &value_attr.attr,
    NULL,
};
static const struct attribute_group demo_group = { .attrs = demo_attrs };

/* ───────────── release:引用计数归零时被调用 ───────────── */
static void demo_release(struct kobject *kobj)
{
    /* 此处 kobject_name 仍可读:名字在 release 之后的清理阶段才释放 */
    printk(KERN_INFO "[kobj_demo] release: %s 引用计数归零,内存释放\n",
           kobject_name(kobj));
    kfree(kobj);                      /* 对应 init 前的 kzalloc */
}

static struct kobj_type demo_ktype = {
    .release       = demo_release,     /* 引用归零 → 调这里 */
    .sysfs_ops     = &kobj_sysfs_ops, /* 让 __ATTR 属性可读可写 */
    .default_attrs = demo_attrs,      /* 注册时自动出现 value 属性文件 */
};

/* ───────────── 三个实验对象 ───────────── */
static struct kobject *parent_kobj;   /* A. 快捷方式创建的父目录 */
static struct kobject *demo_kobj;     /* B. 完整生命周期:alloc→init→add→get→del→put→release */
static struct kobject *auto_kobj;     /* C. 快捷方式创建的子对象 */

static int __init kobj_demo_init(void)
{
    int ret;

    printk(KERN_INFO "[kobj_demo] ===== kobject 生命周期实验开始 =====\n");

    /* ① 快捷方式:在 /sys/kernel 下建父目录 */
    parent_kobj = kobject_create_and_add("kobj_demo", kernel_kobj);
    if (!parent_kobj) {
        printk(KERN_ERR "[kobj_demo] 创建 parent 失败\n");
        return -ENOMEM;
    }
    printk(KERN_INFO "[kobj_demo] 1. /sys/kernel/kobj_demo/ 已创建\n");

    /* ② 完整生命周期第一步:动态分配 */
    demo_kobj = kzalloc(sizeof(*demo_kobj), GFP_KERNEL);
    if (!demo_kobj) {
        kobject_put(parent_kobj);
        return -ENOMEM;
    }
    printk(KERN_INFO "[kobj_demo] 2. kzalloc 分配 kobject\n");

    /* ③ 第二步:init(绑定 ktype) + add(起名,挂到 parent 下) */
    ret = kobject_init_and_add(demo_kobj, &demo_ktype, parent_kobj, "%s", "demo_obj");
    if (ret) {
        printk(KERN_ERR "[kobj_demo] init_and_add 失败:%d\n", ret);
        kobject_put(demo_kobj);       /* 失败回滚:计数归零 → 触发 release → kfree */
        kobject_put(parent_kobj);
        return ret;
    }
    printk(KERN_INFO "[kobj_demo] 3. demo_obj 已挂到 /sys/kernel/kobj_demo/demo_obj\n");
    printk(KERN_INFO "[kobj_demo]    板子上试试: cat /sys/kernel/kobj_demo/demo_obj/value\n");
    printk(KERN_INFO "[kobj_demo]    再试试:     echo 123 > 同上路径, 然后 cat\n");

    /* ④ 引用计数实验:额外拿一次引用,观察计数变化 */
    kobject_get(demo_kobj);
    printk(KERN_INFO "[kobj_demo] 4. kobject_get 后引用计数 = %d(退出时要 put 两次)\n",
           kref_read(&demo_kobj->kref));

    /* ⑤ 快捷方式再建一个子对象 */
    auto_kobj = kobject_create_and_add("auto_obj", demo_kobj);
    if (!auto_kobj) {
        printk(KERN_ERR "[kobj_demo] 创建 auto_obj 失败\n");
        kobject_put(demo_kobj);       /* 归还 get 引用 */
        kobject_put(demo_kobj);       /* 归还创建引用 → release → kfree */
        kobject_put(parent_kobj);
        return -ENOMEM;
    }
    printk(KERN_INFO "[kobj_demo] 5. auto_obj 已创建(引用计数=%d)\n",
           kref_read(&auto_kobj->kref));

    return 0;
}

static void __exit kobj_demo_exit(void)
{
    printk(KERN_INFO "[kobj_demo] ===== 实验结束,逆序拆除 =====\n");

    /* ① 叶子先拆:引用归零 → 自动从 sysfs 摘除 + release + kfree */
    kobject_put(auto_kobj);
    printk(KERN_INFO "[kobj_demo] auto_obj 已释放\n");

    /* ② demo_kobj:先 del(目录消失,对象还在!),再两次 put 才真正释放 */
    kobject_del(demo_kobj);
    printk(KERN_INFO "[kobj_demo] demo_obj 目录已删除,对象还在(引用=%d)\n",
           kref_read(&demo_kobj->kref));

    kobject_put(demo_kobj);           /* 归还 get 引用:2 → 1,release 不触发 */
    printk(KERN_INFO "[kobj_demo] 第1次 put: 计数=%d,release 尚未触发\n",
           kref_read(&demo_kobj->kref));

    kobject_put(demo_kobj);           /* 归零 → demo_release → kfree */
    printk(KERN_INFO "[kobj_demo] 第2次 put: release 已触发,对象已释放\n");

    /* ③ 最后拆父目录 */
    kobject_put(parent_kobj);
    printk(KERN_INFO "[kobj_demo] ===== 全部对象已释放 =====\n");
}

module_init(kobj_demo_init);
module_exit(kobj_demo_exit);

MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("kobject complete lifecycle demo");
MODULE_AUTHOR("kongchen");
