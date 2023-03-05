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

/* module int and exit */
static int __init mod_int(void);
static void __exit mod_exit(void);

u32 module_start_time, module_end_time;

/* module init */
static int __init mod_int(void)
{
    /* Linux jiffizes and time test */
    printk(KERN_INFO "[JIFFIZES TEST]: jiffizes HZ test: system HZ is: %d \n", HZ);
    module_start_time = jiffies;
    return 0;
}

/* module exit */
static void __exit mod_exit(void)
{
    u32 seconds = 0x00;
    module_end_time = jiffies; 
    seconds = (module_end_time - module_start_time) / HZ;
    printk(KERN_INFO "[JIFFIZES TEST]: %s: module time: %d seconds\n", __func__, seconds);
    printk(KERN_INFO "[JIFFIZES TEST]: jiffizes HZ test exit!\n");
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

/*--------------------------------------------------------------------*/
