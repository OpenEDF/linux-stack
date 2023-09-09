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

/* init the tasklet by static method */
void tasklet_fn(unsigned long arg);
DECLARE_TASKLET_OLD(tasklet, tasklet_fn);

/** Log entry point for debugging */
#define ENTER() pr_info("--->: %s\n", __func__)
/** Log exit point for debugging */
#define LEAVE() pr_info("<---: %s\n", __func__)

/* tasklet function */
void tasklet_fn(unsigned long arg)
{
    ENTER();
    pr_info("executing tasklet function: arg = %ld\n", arg);
    LEAVE();
}

static int __init mod_int(void)
{
    ENTER();
    /* Linux firmware download test */
    printk(KERN_INFO "linux tasklet sm test...\n");

    /* scheduing task to tasklet */
    tasklet.data = 0x01;
    tasklet_schedule(&tasklet);

    pr_info("module init done!\n");
    LEAVE();
    return 0;
}

static void __exit mod_exit(void)
{
    ENTER();

    /* kill the tasklet */
    tasklet_kill(&tasklet);
    printk(KERN_INFO "linux firamware file test exit!\n");
    LEAVE();
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

/*--------------------------------------------------------------------*/
