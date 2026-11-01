#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/rcupdate.h>
#include <linux/kthread.h>
#include <linux/delay.h>

// shared data structure
struct foo {
    int a;
    struct rcu_head rcu;
};

// global RCU protected pointer
static struct foo *g_ptr;

// reader thread function
static int myrcu_reader_thread(void *data)
{
    struct foo *p = NULL;
    while (1) {
        msleep(200);

        rcu_read_lock();

        p = rcu_dereference(g_ptr);
        if (p) {
            printk("%s: read a=%d\n", __func__, p->a);
        }

        rcu_read_unlock();
    }
    return 0;
}

// rcu callback function
static void myrcu_del(struct rcu_head *rh)
{
    struct foo *p = container_of(rh, struct foo, rcu);
    printk("%s: free old data, a= %d\n", __func__, p->a);
    kfree(p);
}

// write thread
static int myrcu_writer_thread(void *p)
{
    struct foo *new_ptr;
    struct foo *old;
    int value = (unsigned long)p;

    while (1) {
        msleep(400);

        new_ptr = kmalloc(sizeof(struct foo), GFP_KERNEL);
        if (!new_ptr) {
            continue;
        }

        old = g_ptr;

        printk("%s: write to new %d\n", __func__, value);
        *new_ptr = *old;
        new_ptr->a = value;

        rcu_assign_pointer(g_ptr, new_ptr);

        call_rcu(&old->rcu, myrcu_del);

        value++;
    }
    return 0;
}

// Initialize function
static int __init my_test_init(void)
{
    struct task_struct *reader_thread;
    struct task_struct *writer_thread;
    int value =5;

    printk("RCU demo: module init\n");

    g_ptr = kzalloc(sizeof(struct foo), GFP_KERNEL);
    if (!g_ptr)  {
        return -ENOMEM;
    }

    reader_thread = kthread_run(myrcu_reader_thread, NULL, "rcu_reader");
    if (IS_ERR(reader_thread)) {
        kfree(g_ptr);
        return PTR_ERR(reader_thread);
    }

    writer_thread = kthread_run(myrcu_writer_thread, (void *)(unsigned long)value,
                            "rcu_writer");
    if (IS_ERR(writer_thread)) {
        kfree(g_ptr);
        return PTR_ERR(writer_thread);
    }

    return 0;
}

static void __exit my_test_exit(void)
{
    printk("RCU demo: goodbye\n");
    if (g_ptr) {
        kfree(g_ptr);
    }
}

// Create a module registration macro
module_init(my_test_init);
module_exit(my_test_exit);

// Create a module declaration macro
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("Module Driver");
MODULE_AUTHOR("kongchen");
