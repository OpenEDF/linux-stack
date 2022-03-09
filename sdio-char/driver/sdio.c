#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/err.h>

// CHAR device Header
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

// SDIO device Header
#include <linux/mmc/core.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio_func.h>

/*
 * define the sdio function id and manfacturer cdoe
 * */
#define TEST_SDIO_VENDOR_ID 0x0000
#define TEST_SDIO_DEVICE_ID 0x0000

/*
 * define use device tree
 * */
#define USE_DEVICE_TREE

/*
 * driver name
 * */
#define SDIO_DRIVER_NAME         "rpi-sdio"
#define SDIO_NAME_DEV_TREE_MATCH "xxx,rpi-sdio"
#define SDIO_DRIVER_CNT          1

/*
 * SDIO char device structer
 * */
typedef struct {
    dev_t devid;        /* device id */
    struct cdev cdev;   /* cdev */
    struct class *class;
    struct device *device;
    struct device_node *nd;
    int major;              /* device major number */
}test_sdio_dev;

static test_sdio_dev sdio_dev;

/*
 * name : test_sdio_open
 * brief: sdio device file open function
 * param: indoe: device indoe
 * param: filp: device operation file
 * ret  : file inode open state
 */
static int test_sdio_open (struct inode *inode, struct file *filp)
{
    int ret = 0;
    pr_info("%s: Driver open()\n", SDIO_DRIVER_NAME);

    return ret;
}

/*
 * name : test_sdio_read
 * brief: device read
 * param: filp: file operation
 * param: buf: buffer for data transmit
 * param: loff: loff
 * ret  : read state
 */
static ssize_t test_sdio_read (struct file *filp, char __user *buf, size_t len, loff_t *loff)
{
    ssize_t ret = 0;
    pr_info("%s: Driver read()\n", SDIO_DRIVER_NAME);

    return ret;
}

/*
 * name : test_sdio_write
 * brief: device write
 * param: filp: file operation
 * param: buf: buffer for data transmit
 * param: loff: loff
 * ret  : write state
 */
static ssize_t test_sdio_write (struct file *filp, const char __user *buf, size_t len, loff_t * loff)
{
    ssize_t ret = 0;
    pr_info("%s: Driver write()\n", SDIO_DRIVER_NAME);

    return ret;
}

/*
 * name : test_sdio_unlocked_toctl
 * brief: ioctl operation
 * param: filp: file operation
 * param: cmd: ioctl command
 * param: argc: the command argcment
 * ret  : operation state
 */
static long test_sdio_unlocked_ioctl (struct file *filp, unsigned int cmd, unsigned long argc)
{
    long ret = 0;
    pr_info("%s: Driver ioctl()\n", SDIO_DRIVER_NAME);

    return ret;
}

/*
 * name : test_sdio_release
 * brief: file close
 * param: indoe: device noide
 * param: file: file operation
 * ret  : close device state
 */
static int test_sdio_release (struct inode *inode, struct file *filp)
{
    int ret = 0;
    pr_info("%s: Driver close()\n", SDIO_DRIVER_NAME);

    return ret;
}

/*
 * File operation structure
 * */
static struct file_operations test_sdio_fops = {
    .owner = THIS_MODULE,
    .open = test_sdio_open,
    .read = test_sdio_read,
    .write = test_sdio_write,
    .unlocked_ioctl = test_sdio_unlocked_ioctl,
    .release = test_sdio_release,
};

/*
 * sdio device id table
 * */
static const struct sdio_device_id test_sdio_id_table[] = {
    { SDIO_DEVICE(TEST_SDIO_VENDOR_ID, TEST_SDIO_DEVICE_ID) },
    {},
};

#ifdef USE_DEVICE_TREE
/*
 * sdio device tree match of table
 * */
static const struct of_device_id test_sdio_of_table[] = {
    { .compatible = SDIO_NAME_DEV_TREE_MATCH },   // device tree
    {},
};
#endif

/*
 * name : test_sdio_probe
 * brief: sdio probe function, This functions is called by kernel when the
 *        driver provied vendor and device IDs arm match. All the initialization
 *        work is done here.
 * param: sdio_func: sdio function devices
 * param: sdio_device_id: sdio device id table
 * ret  : sdio driver probe state
 */
static int test_sdio_probe(struct sdio_func *func, const struct sdio_device_id *id)
{
    int ret = 0;

    /* debug sdio information */
    pr_info("%s: Info: vendor=0x%4.04X device =0x%4.04X class=%d function=%d\n", SDIO_DRIVER_NAME, \
            func->vendor, func->device, func->class, func->num);

    /* sdio enabel function */
    sdio_claim_host(func);


    /* Request dynamic allocation of a device major number */
    if (sdio_dev.major) {
        sdio_dev.devid = MKDEV(sdio_dev.major, 0);
        register_chrdev_region(sdio_dev.devid, SDIO_DRIVER_CNT, SDIO_DRIVER_NAME);
    } else {
        if ((ret = alloc_chrdev_region(&sdio_dev.devid, 0, SDIO_DRIVER_CNT, SDIO_DRIVER_NAME)) < 0) {
            return ret;
        }
        sdio_dev.major = MAJOR(sdio_dev.devid);
    }

    /* Create class */
    sdio_dev.class = class_create(THIS_MODULE, SDIO_DRIVER_NAME);
    if (IS_ERR(sdio_dev.class)) {
        unregister_chrdev_region(sdio_dev.devid, SDIO_DRIVER_CNT);
        return PTR_ERR(sdio_dev.class);
    }

    /* Create device */
    SDIO_NAME_DEV_TREE_MATCH.device = device_create(sdio_dev.class, NULL, sdio_dev.devid, NULL, SDIO_DRIVER_NAME); // sysfs
    if (IS_ERR(sdio_dev.device)) {
        class_destroy(sdio_dev.class);
        unregister_chrdev_region(sdio_dev.devid, SDIO_DRIVER_CNT);
        return PTR_ERR(sdio_dev.device);
    }

    /* Register char device, bind file operations */
    cdev_init(&sdio_dev.cdev, &test_sdio_fops);
    if ((ret = cdev_add(&sdio_dev.cdev, sdio_dev.devid, SDIO_DRIVER_CNT)) < 0)
    {
        device_destroy(sdio_dev.class, SDIO_DRIVER_CNT);
        class_destroy(sdio_dev.class);
        unregister_chrdev_region(sdio_dev.devid, SDIO_DRIVER_CNT);
        return ret;
    }

    pr_info("%s: device registerted.\n", SDIO_DRIVER_NAME);



    return ret;
}

/*
 * name : test_sdio_remove
 * brief: sdio remove function
 * param: sdio_func: sdio function devices
 * ret  : none
 */
static void test_sdio_remove(struct sdio_func *func)
{
    /* Deleted device */
    cdev_del(&sdio_dev.cdev);
    device_destroy(sdio_dev.class, sdio_dev.devid);

    /* Unregister device */
    class_destroy(sdio_dev.class);
    unregister_chrdev_region(sdio_dev.devid, SDIO_DRIVER_CNT);
    pr_info("%s: device unregisterted.\n", SDIO_DRIVER_NAME);

    /* Remove the sdio device */
    sdio_claim_host(func);
    sdio_release_irq(func);
    sdio_disable_func(func);
    sdio_release_host(func);
    pr_info("%s: device remove.\n", SDIO_DRIVER_NAME);
}

/*
 * sdio driver struct
 * */
struct sdio_driver test_sdio_driver = {
    .name = SDIO_DRIVER_NAME,
    .id_table = test_sdio_id_table,
    .probe = test_sdio_probe,
    .remove = test_sdio_remove,
    .drv = {
        .name = SDIO_DRIVER_NAME,
        .owner = THIS_MODULE,
#ifdef USE_DEVICE_TREE
        .of_match_table = test_sdio_of_table,
#endif
    },
};

/*
 * module init function and exit function
 * */
static int __init sdio_init(void);
static void __exit sdio_exit(void);

/*
 * name : sdio_init
 * brief: linux module init func
 * param: none
 * ret  : initial state
 */
static int __init sdio_init(void)
{
    int ret = 0;
    pr_info("%s: Init driver module.\n", SDIO_DRIVER_NAME);

    /* register sdio driver */
    ret = sdio_register_driver(&test_sdio_driver);
    if (ret) {
        pr_err("%s: Unable to register as sdio driver.\n", SDIO_DRIVER_NAME);
        return ret;
    }
    else {
        pr_err("%s: Success register as sdio driver.\n", SDIO_DRIVER_NAME);
    }

    return ret;
}

/*
 * name : sdio_exit
 * brief: linux module exit func
 * param: none
 * ret  : none
 */
static void __exit sdio_exit(void)
{
    /* unregister sdio driver */
    sdio_unregister_driver(&test_sdio_driver);

    pr_info("Exit %s driver module.\n", SDIO_DRIVER_NAME);
}

/*
 * module init and exit register
 * */
module_init(sdio_init);
module_exit(sdio_exit);

/*
 * module parameter declare
 * */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Macro, <makermuyi@gmail.com>");
MODULE_DESCRIPTION("Raspbeery pi sdio test");
MODULE_VERSION("0.0.1");
//MODULE_SUPPORT_DEVICE("");
//MODULE_FIRMWARE("")
/*------------------------ end -----------------------*/
