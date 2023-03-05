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
#include <linux/proc_fs.h>
#include <linux/kobject.h>
#include <linux/err.h>
#include <linux/ioctl.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/timer.h>

/* timer variable */
#define TIMEOUT 5000   //milliseconds
static struct timer_list etx_timer;
static unsigned int count = 0x00;

/* timer callback function, This will be called when the timer expires */
void timer_callback(struct timer_list *data)
{
    /* do your timer stuff here */
    pr_info("%s: Timer callback function called [%d]\n", __func__, count++);

    /* re-enable timer */
    mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));
}

/* module int and exit */
static int __init mod_int(void);
static void __exit mod_exit(void);

/* module init */
static int __init mod_int(void)
{
    /* Linux timer test */
    printk(KERN_INFO "[TIMER TEST]: timer test: \n");

    /* setup the timer to call timer callback function */
    timer_setup(&etx_timer, timer_callback, 0x00);
    mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));
    return 0;
}

/* module exit */
static void __exit mod_exit(void)
{
    del_timer(&etx_timer);
    printk(KERN_INFO "[TIMER TEST]: timer test exit! \n");
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

/*--------------------------------------------------------------------*/
