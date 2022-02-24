#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

// init function
static int __init hello_init(void)
{
    printk(KERN_EMERG, "[ KERN_EMERG ] Hello World Module Init.\n");
    printk("[ default ] Hello World Module Init.\n");
}

// exit function
static int __exit hello_exit(void)
{
    printk("[ default ] Hello World Module Exit.\n");
    return 0;
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL2");
MODULE_AUTHOR("Macro");
MODULE_DESCRIPTION("HELLO WORLD TEST MODULE");
MODULE_ALIAS("test_module");

