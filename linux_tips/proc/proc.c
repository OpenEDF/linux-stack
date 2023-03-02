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
#include <linux/proc_fs.h>

char etx_array[20] = "try_proc_array\n";
static int len = 0x01;
static struct proc_dir_entry *parent;

/* ------------- proc files ---------------- */
static int proc_open(struct inode *inode, struct file *file);
static ssize_t proc_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset);
static ssize_t proc_write(struct file *filp, const char __user *buffer, size_t length, loff_t *offset);
static int proc_release(struct inode *inode, struct file *filp);

/* proc operation structer */
static struct proc_ops proc_fops = {
    .proc_open = proc_open,
    .proc_read = proc_read,
    .proc_write = proc_write,
    .proc_release = proc_release,
};

static int proc_open(struct inode *inode, struct file *file)
{
    pr_info("proc file opend......\n");
    return 0;
}

static ssize_t proc_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset)
{
    pr_info("proc file read......\n");
    if(len) {
        len = 0x00;
    } else {
        len = 0x01;
        return 0;
    }

    if(copy_to_user(buffer, etx_array, 20)) {
        pr_err("data send: err!\n");
    }
    return length;
}

static ssize_t proc_write(struct file *filp, const char __user *buffer, size_t length, loff_t *offset)
{
    pr_info("proc file wrote......\n");
    if(copy_from_user(etx_array, buffer, len)) {
        pr_err("data write: err!\n");
    }
    return length;
}

static int proc_release(struct inode *inode, struct file *filp)
{
    pr_info("proc file released......\n");
    return 0;
}

static int __init mod_int(void)
{
    /* Linux proc filesystem test */
    printk(KERN_INFO "[PROC TEST]: Linux proc file test:\n");

    /* Create proc directory. it will crate a dirextory under "/proc" */
    parent = proc_mkdir("etx", NULL);
    if(parent == NULL) {
        pr_info("error creating proc entry!\n");
        return -1;
    }

    /* create proc entry under "/proc/etx/" */
    proc_create("etx_proc", 0666, parent, &proc_fops);
    pr_info("module insert...done!\n");
    return 0;
}

static void __exit mod_exit(void)
{
    proc_remove(parent);
    printk(KERN_INFO "[PROC TEST]: Linux proc file test exit!\n");
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

/*--------------------------------------------------------------------*/
