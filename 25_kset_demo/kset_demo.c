#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/configfs.h>
#include <linux/kernel.h>
#include <linux/kobject.h>

// Define three the variable of kobject
struct kobject *mykobject01;
struct kobject *mykobject02;
struct kset *mykset;
struct kobj_type mytype;

struct kobj_type mytype;

static int mykobj_init(void)
{
    int ret;
    mykset = kset_create_and_add("mykset", NULL, NULL);
    mykobject01 = kzalloc(sizeof(struct kobject), GFP_KERNEL);
    mykobject01->kset = mykset;

    ret = kobject_init_and_add(mykobject01, &mytype, NULL, "%s", "mykobject01");

    mykobject02 = kzalloc(sizeof(struct kobject), GFP_KERNEL);
    mykobject02->kset = mykset;

    ret = kobject_init_and_add(mykobject02, &mytype, NULL, "%s", "mykobject02");
    return 0;
}

static void mykobj_exit(void)
{
    kobject_put(mykobject01);
    kobject_put(mykobject02);
}

module_init(mykobj_init);
module_exit(mykobj_exit);

MODULE_LICENSE("GPL");
