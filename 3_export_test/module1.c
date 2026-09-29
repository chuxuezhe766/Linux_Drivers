#include <linux/init.h>
#include <linux/module.h>

// Define global variables
int num = 10;           // Define the global variable num
EXPORT_SYMBOL(num);     // Export the global variable num

// Define add function
int add(int a, int b)
{
    return a + b;
}

// Export add function
EXPORT_SYMBOL(add);

// Driver entry function
static int __init math_init(void)
{
    printk("math_module init\n");
    return 0;
} 

// Driver exit function
static void __exit math_exit(void)
{
    printk("math_module exit\n");
}

// Create a module registration macro
module_init(math_init);
module_exit(math_exit);

// Create a module declaration macro
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("module1 Driver");
MODULE_AUTHOR("kongchen");
