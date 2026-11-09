#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/configfs.h>
#include <linux/kernel.h>
#include <linux/kobject.h>

// Define the variable 
struct kobject *mykobject01;
struct kobject *mykobject02;
struct kobject *mykobject03;

// Define the struct mytype
struct kobj_type mytype;

// Initialize
static int mykobj_init(void)
{
    int ret;
    mykobject01 = kobject_create_and_add("mykobject01", NULL);
    mykobject02 = kobject_create_and_add("mykobject02", mykobject01);

    // Create kobject
    mykobject03 = kzalloc(sizeof(struct kobject), GFP_KERNEL);
    ret = kobject_init_and_add(mykobject03, &mytype, NULL, "%s", "mykobject03");

    return 0;
}

// exit
static void mykobj_exit(void)
{
    kobject_put(mykobject02);
    kobject_put(mykobject03);
}

// Create a module registration macro
module_init(mykobj_init);
module_exit(mykobj_exit);

// Create a module declaration macro
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("Module Driver");
MODULE_AUTHOR("kongchen");
