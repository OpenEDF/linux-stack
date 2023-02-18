#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kernel.h>

extern struct module __this_module;

static int __init mod_int(void)
{
    /* set by default based on the mdoule fine name */
    printk(KERN_INFO "name = %s\n", THIS_MODULE->name);
    printk(KERN_INFO "version = %s\n", THIS_MODULE->version);
    printk(KERN_INFO "version = %s\n", (&__this_module)->version);
    return 0;
}

static void __exit mod_exit(void)
{
    printk(KERN_INFO "exit\n");
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("CPL");
MODULE_VERSION("1.0");
