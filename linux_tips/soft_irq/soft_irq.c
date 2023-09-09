/*--------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/err.h>
#include <linux/interrupt.h>

/* softirq function */
void soft_irq_fn(struct softirq_action *action);

/** Log entry point for debugging */
#define ENTER() pr_info("--->: %s\n", __func__)
/** Log exit point for debugging */
#define LEAVE() pr_info("<---: %s\n", __func__)

/* softirq function */
void soft_irq_fn(struct softirq_action *action)
{
    ENTER();
    pr_info("executing softirq function!\n");
    LEAVE();
}

static int __init mod_int(void)
{
    ENTER();
    /* Linux firmware download test */
    printk(KERN_INFO "linux softirq test...\n");

    /* open softirq */
    open_softirq(MACRO_TEST_SOFTIRQ, soft_irq_fn);

    /* raise teh softirq */
    raise_softirq(MACRO_TEST_SOFTIRQ);

    pr_info("module init done!\n");
    LEAVE();
    return 0;
}

static void __exit mod_exit(void)
{
    ENTER();
    printk(KERN_INFO "linux softirq test exit!\n");
    LEAVE();
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

/*--------------------------------------------------------------------*/
