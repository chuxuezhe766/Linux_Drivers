#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/configfs.h>
#include <linux/kernel.h>
#include <linux/kobject.h>

// Define three the variable of kobject
struct kobject *mykobject01;
struct kobject *mykobject02;
struct kobject *mykobject03;

struct kobj_type mytype;

static int mykobj_init(void)
{
    int ret;
    mykobject01 = kobject_create_and_add("myobject01", NULL);
    printk("mykobject01 kref is %d\n", mykobject01->kref.refcount.refs.counter);

    mykobject02 = kobject_create_and_add("mykobject02", mykobject01);
    printk("mykobject01 kref is %d\n", mykobject01->kref.refcount.refs.counter);
    printk("mykobject02 kref is %d\n", mykobject02->kref.refcount.refs.counter);

    mykobject03 = kzalloc(sizeof(struct kobject), GFP_KERNEL);

    ret = kobject_init_and_add(mykobject03, &mytype, NULL, "%s", "mykobject03");
    printk("mykobject03 kref is %d\n", mykobject03->kref.refcount.refs.counter);

    return 0;
   
}

static void mykobj_exit(void)
{
    printk("mykobject01 kref is %d\n", mykobject01->kref.refcount.refs.counter);
    printk("mykobject02 kref is %d\n", mykobject02->kref.refcount.refs.counter);
    printk("mykobject03 kref is %d\n", mykobject03->kref.refcount.refs.counter);
    
    kobject_put(mykobject01);
    printk("mykobject01 kref is %d\n", mykobject01->kref.refcount.refs.counter);
    printk("mykobject02 kref is %d\n", mykobject02->kref.refcount.refs.counter);
    printk("mykobject03 kref is %d\n", mykobject03->kref.refcount.refs.counter);

    kobject_put(mykobject02);
    printk("mykobject01 kref is %d\n", mykobject01->kref.refcount.refs.counter);
    printk("mykobject02 kref is %d\n", mykobject02->kref.refcount.refs.counter);
    printk("mykobject03 kref is %d\n", mykobject03->kref.refcount.refs.counter);

    kobject_put(mykobject03);
    printk("mykobject01 kref is %d\n", mykobject01->kref.refcount.refs.counter);
    printk("mykobject02 kref is %d\n", mykobject02->kref.refcount.refs.counter);
    printk("mykobject03 kref is %d\n", mykobject03->kref.refcount.refs.counter);

}

module_init(mykobj_init);
module_exit(mykobj_exit);

MODULE_LICENSE("GPL");
