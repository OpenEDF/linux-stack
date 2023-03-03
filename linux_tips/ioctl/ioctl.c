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

/* create the ioctl command in driver */
#define WR_VALUE    _IOW('a', 'a', uint32_t*)
#define RD_VALUE    _IOR('a', 'b', uint32_t*)

uint32_t value = 0x00;
dev_t dev = 0x00;
static struct class *dev_class;
static struct cdev etx_cdev;

/* function prototypes */
static ssize_t read_t(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t write_t(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static int open_t(struct inode *inode, struct file *filp);
static long unlocked_ioctl_t(struct file *filp, unsigned int cmd, unsigned long arg);
static int release_t(struct inode *inode, struct file *filp);

/* thsi function will be called when we read the device file */
static ssize_t read_t(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    pr_info("read function call\n");
    return 0;
}

/* thsi function will be called when we write the device file */
static ssize_t write_t(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    pr_info("write function call\n");
    return len;
}

/* thsi function will be called when we open the device file */
static int open_t(struct inode *inode, struct file *filp)
{
    pr_info("device file open\n");
    return 0;
}

/* thsi function will be called when we IOCTL on the device file */
static long unlocked_ioctl_t(struct file *filp, unsigned int cmd, unsigned long arg)
{
    pr_info("device file ioctl opeation\n");
    switch(cmd) {
        case WR_VALUE:
            if(copy_from_user(&value, (uint32_t *)arg, sizeof(value))) {
                pr_err("data write: error!\n"); 
            }
            pr_info("Value = %d\n", value);
            break;
        case RD_VALUE:
            if(copy_to_user((uint32_t *)arg, &value, sizeof(value))) {
                pr_err("data read: error!\n"); 
            }
            break;
        default:
            pr_info("default\n");
            break;
    }   
    return 0;
}

/* thsi function will be called when we release the device file */
static int release_t(struct inode *inode, struct file *filp)
{
    pr_info("device file release\n");
    return 0;
}

/* file operation structure*/
static struct file_operations fops = {
	.owner          = THIS_MODULE,
	.read           = read_t,
	.write          = write_t,
	.open           = open_t,
	.unlocked_ioctl = unlocked_ioctl_t,
	.release        = release_t,
};

/* module init */
static int __init mod_int(void)
{
    /* Linux proc filesystem test */
    printk(KERN_INFO "[IOCTL TEST]: Linux proc file test:\n");

    /* allocate major number */
    if((alloc_chrdev_region(&dev, 0, 1, "etx_dev")) < 0x00) {
        pr_err("cannot allocate major number\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

    /* create cdev strurcture */
    cdev_init(&etx_cdev, &fops);

    /* createing struct class */
    if(IS_ERR(dev_class = class_create(THIS_MODULE, "etx_cLass"))) {
        pr_err("cannot create the struct class\n");
        goto r_class;
    }

    /* creating device */
    if(IS_ERR(device_create(dev_class, NULL, dev, NULL, "etx_device"))){
        pr_err("cannot create device\n");
        goto r_device;
    }
    pr_info("device driver insert...done!\n");
    return 0;
r_class:
    class_destroy(dev_class);
r_device:
    unregister_chrdev_region(dev, 1);
    return -1;
}

/* module exit */
static void __exit mod_exit(void)
{
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "[IOCTL TEST]: Linux proc file test exit!\n");
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

/*--------------------------------------------------------------------*/
