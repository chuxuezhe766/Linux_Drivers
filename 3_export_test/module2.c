#include <linux/init.h>
#include <linux/module.h>

// Declare the external variable 
extern int num;
// Declare the external function
extern int add(int a, int b);

// Driver entry function
static int __init hello_init(void)
{
    static int sum;
    printk("num = %d\n", num);
    sum = add(3, 4);
    printk("sum = %d\n", sum);
    return 0;
}

// Driver exit function
static void __exit hello_exit(void)
{
    printk("Goodbye hello module\n");
}

// Create a module registration macro
module_init(hello_init);
module_exit(hello_exit);

// Create a module declaration macro
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("module2");
MODULE_AUTHOR("kongchen");
