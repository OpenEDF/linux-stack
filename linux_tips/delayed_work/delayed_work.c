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
#include <linux/workqueue.h>
#include <linux/delay.h>

/* init the workqueue by dynamic method */
void workqueue_fn(struct work_struct *work);
static int count = 0x00;

/** Log entry point for debugging */
#define ENTER() pr_info(" ---> %s\n", __func__)
/** Log exit point for debugging */
#define LEAVE() pr_info(" <--- %s\n", __func__)

/* create work by dynamic method */
static struct delayed_work *test_delay_work = NULL;

/* workqueue function */
void workqueue_fn(struct work_struct *work)
{
    ENTER();
    pr_info("executing workqueue function! count: %d\n", count);
    count++;
    schedule_delayed_work(test_delay_work, msecs_to_jiffies(5000));
    LEAVE();
}

static int __init mod_int(void)
{
    ENTER();
    /* Linux firmware download test */
    printk(KERN_INFO "linux own workqueue test...\n");

    /* scheduing task to workqueue */
   test_delay_work = kmalloc(sizeof(struct delayed_work), GFP_KERNEL);
    if (test_delay_work == NULL) {
        pr_info("cannot kmalloc memory!\n");
        goto out;
    }
    INIT_DELAYED_WORK(test_delay_work, workqueue_fn);

    /* queue work */
    schedule_delayed_work(test_delay_work, msecs_to_jiffies(1000));
out:
    pr_info("module init done!\n");
    LEAVE();
    return 0;
}

static void __exit mod_exit(void)
{
    ENTER();
    /* queue work */
    cancel_delayed_work(test_delay_work);
    printk(KERN_INFO "linux workqueue test test exit!\n");
    LEAVE();
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

/*--------------------------------------------------------------------*/
