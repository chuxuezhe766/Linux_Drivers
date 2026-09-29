#include <linux/init.h>
#include <linux/module.h>

// define variate
static int number;      // int variable
static char *name;      // char variable
static int para[8];     // array
static char str1[10];   // character string
static int n_para;      // number of array elements

// pass parameters using macros
module_param(number, int, S_IRUSR | S_IWUSR);           // Pass int parameters
module_param(name, charp, S_IRUGO);                     // Pass char variable
module_param_array(para, int, &n_para, S_IRUGO);        // Pass array
module_param_string(str, str1, sizeof(str1), S_IRUGO);  // Pass character string

// Driver entry function
static int __init parameter_init(void)
{
    static int i;

    // Print the passed parameters
    printk(KERN_EMERG "Number: %d\n", number);
    printk(KERN_EMERG "Name: %s\n", name);

    for (i = 0; i < n_para; i ++) {
        printk(KERN_EMERG "Para[%d]: %d\n", i, para[i]);
    }

    printk(KERN_EMERG "String: %s\n", str1);

    return 0;
}

// Driver exit function
static void __exit parameter_exit(void)
{
    printk(KERN_EMERG "parameter_exit\n");
}

// Create a module registration macro
module_init(parameter_init);
module_exit(parameter_exit);

// Create a module declaration macro
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("Parameter Driver");
MODULE_AUTHOR("kongchen");
