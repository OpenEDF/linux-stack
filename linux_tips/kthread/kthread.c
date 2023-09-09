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
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>

/* kernel thread function */
int kthread_fn(void *pv);
static struct task_struct *etx_thread;

/** Log entry point for debugging */
#define ENTER() pr_info("--->: %s\n", __func__)
/** Log exit point for debugging */
#define LEAVE() pr_info("<---: %s\n", __func__)

/* kernel thread function */
int kthread_fn(void *pv)
{
    int index = 0x00;
    ENTER();
    while (!kthread_should_stop()) {
        pr_info("executing kthread function %d\n", index++);
        msleep(1000);
    }
    LEAVE();
    return 0x00;
}

static int __init mod_int(void)
{
    ENTER();
    /* Linux firmware download test */
    printk(KERN_INFO "linux softirq test...\n");

    /* create thread */
    etx_thread = kthread_create(kthread_fn, NULL, "macro test");
    if (etx_thread) {
        pr_info("etx thread create succefully!\n");
        wake_up_process(etx_thread);
    } else {
        pr_info("cannot created kthread!\n");
        goto out;
    }

out:
    pr_info("module init done!\n");
    LEAVE();
    return 0;
}

static void __exit mod_exit(void)
{
    ENTER();
    kthread_stop(etx_thread);
    printk(KERN_INFO "linux softirq test exit!\n");
    LEAVE();
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

/*--------------------------------------------------------------------*/
