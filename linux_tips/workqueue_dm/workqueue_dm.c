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

/* init the workqueue by dynamic method */
void workqueue_fn(struct work_struct *work);

/** Log entry point for debugging */
#define ENTER() pr_info("--->: %s\n", __func__)
/** Log exit point for debugging */
#define LEAVE() pr_info("<---: %s\n", __func__)

/* workqueue function */
void workqueue_fn(struct work_struct *work)
{
    ENTER();
    pr_info("executing workqueue function!\n");
    LEAVE();
}

/* create work by dynamic method */
static struct work_struct *workqueue = NULL;

static int __init mod_int(void)
{
    ENTER();
    /* Linux firmware download test */
    printk(KERN_INFO "linux tasklet sm test...\n");

    /* scheduing task to workqueue */
    workqueue = kmalloc(sizeof(struct work_struct), GFP_KERNEL);
    if (workqueue == NULL) {
        pr_info("cannot kmalloc memory!\n");
        goto out;
    }
    INIT_WORK(workqueue, workqueue_fn);
    schedule_work(workqueue);

out:
    pr_info("module init done!\n");
    LEAVE();
    return 0;
}

static void __exit mod_exit(void)
{
    ENTER();
    printk(KERN_INFO "linux workqueue test test exit!\n");
    LEAVE();
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

/*--------------------------------------------------------------------*/
