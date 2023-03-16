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
#include <linux/kprobes.h>

/* static variable */
struct kretprobe;
static struct kretprobe retprobe;
struct user_data {
    ktime_t time_stamp;
};

static int entry_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
    struct user_data *user_data;
    if (!current->mm) return 1;
    user_data = (struct user_data *)ri->data;
    user_data->time_stamp = ktime_get();
    return 0;
}

static int ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
    int retval = regs_return_value(regs);
    struct user_data *user_data = (struct user_data *)ri->data;
    int delta;
    ktime_t now;

    now = ktime_get();
    delta = ktime_to_ns(ktime_sub(now, user_data->time_stamp));
    printk(KERN_INFO "_printk returned %d and took %d ns to execute\n", retval, delta);
    return 0;
}

/* module int and exit */
static int __init mod_int(void);
static void __exit mod_exit(void);

/* module init */
static int __init mod_int(void)
{
    int retval = 0x00;
    /* Linux kprobe test */
    printk(KERN_INFO "[KPROBE TEST]: %s In init\n", __func__);
    retprobe.entry_handler = entry_handler;
    retprobe.kp.symbol_name = "_printk";
    retprobe.handler = ret_handler;
    retprobe.data_size = sizeof(struct user_data);
    retprobe.maxactive = 20;
    retval = register_kretprobe(&retprobe);

    if(retval < 0) {
        printk(KERN_INFO "[KPROBE TEST]: register ketprobe failed, return: %d\n", retval);
    }
    printk(KERN_INFO "[KPROBE TEST]: register ketprobe done.\n");
    printk(KERN_INFO "[KPROBE TEST]: _printk execute time test.\n");
    printk(KERN_INFO "[KPROBE TEST]: _printk execute time test.\n");
    return retval;
}

/* module exit */
static void __exit mod_exit(void)
{
    unregister_kretprobe(&retprobe);
    printk(KERN_INFO "[KPROBE TEST]: %s In exit\n", __func__);
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

/*--------------------------------------------------------------------*/
