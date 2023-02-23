#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kernel.h>

int export_var = 0x00;
void export_shared_func(void)
{
    printk(KERN_INFO "[EXPORT TEST]: export function has be called.\n");
    export_var++;
}
EXPORT_SYMBOL(export_var);
EXPORT_SYMBOL(export_shared_func);

static int __init mod_int(void)
{
    /* Linux module export the function or vailable for other module */
    printk(KERN_INFO "[EXPORT TEST]: Linux export function and variable test:\n");
    export_shared_func();
    printk(KERN_INFO "[EXPORT TEST]: export_var = %d\n", export_var);
    return 0;
}

static void __exit mod_exit(void)
{
    printk(KERN_INFO "[EXPORT TEST]: Linux export function and variable test exit!\n");
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("CPL");
MODULE_VERSION("1.0");
