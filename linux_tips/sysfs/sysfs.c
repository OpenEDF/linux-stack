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

volatile int etx_value = 0;
struct kobject *kobj_ref;

/* ------------- sysfs files ---------------- */
static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
struct kobj_attribute etx_attr = __ATTR(etx_value, 0660, sysfs_show, sysfs_store);

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    pr_info("sysfs show: read!\n");
    return sprintf(buf, "%d", etx_value);
}

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    pr_info("sysfs show: write|\n");
    sscanf(buf, "%d", &etx_value);
    return count;    
}

static int __init mod_int(void)
{
    /* Linux sysfs file and folder creat/deleted/remove/link test */
    printk(KERN_INFO "[EXPORT TEST]: Linux sysfs file test:\n");
    kobj_ref = kobject_create_and_add("etx_sysfs", kernel_kobj);
    
    if(sysfs_create_file(kobj_ref, &etx_attr.attr)) {
        pr_err("cannot create sysfs file...\n");
        goto r_sysfs;
    }
    pr_info("module init done!\n");
    return 0;
r_sysfs:
    kobject_put(kobj_ref);
    sysfs_remove_file(kernel_kobj, &etx_attr.attr);
    return -1;
}

static void __exit mod_exit(void)
{
    kobject_put(kobj_ref);
    sysfs_remove_file(kernel_kobj, &etx_attr.attr);
    printk(KERN_INFO "[EXPORT TEST]: Linux sysfs file test exit!\n");
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

/*--------------------------------------------------------------------*/
