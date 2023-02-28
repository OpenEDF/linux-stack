/*--------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kernel.h>

static int __init mod_int(void)
{
    /* Linux sysfs file and folder creat/deleted/remove/link test */
    printk(KERN_INFO "[EXPORT TEST]: Linux sysfs file test:\n");
    return 0;
}

static void __exit mod_exit(void)
{
    printk(KERN_INFO "[EXPORT TEST]: Linux sysfs file test exit!\n");
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("CPL");
MODULE_VERSION("1.0");

/*--------------------------------------------------------------------*/
