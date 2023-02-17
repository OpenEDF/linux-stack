#include <linux/module.h>
#include <linux/printk.h>
#include <linux/cpumask.h>

static int __init mod_int(void)
{
    pr_info("number of online cpus is %d\n", num_online_cpus());
    return 0;
}

static void __exit mod_exit(void)
{
    pr_info("goodbly!\n");
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("CPL");
