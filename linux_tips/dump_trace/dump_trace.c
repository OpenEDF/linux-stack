#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kernel.h>

static int __init mod_int(void)
{
    printk(KERN_INFO "DUMP STACK: START\n");
    dump_stack();
    printk(KERN_INFO "DUMP STACK: END\n");
    return 0;
}

static void __exit mod_exit(void)
{
    printk(KERN_INFO "DUMP STACK: GOODBYE\n");
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("CPL");
MODULE_VERSION("1.0");
