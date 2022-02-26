#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

// CHAR device Header
#include <linux/fs.h>

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
#define TEST_SDIO_VENDOR_ID 0x0001
#define TEST_SDIO_DEVICE_ID 0x0000

/*
 * define use device tree
 * */
#define USE_DEVICE_TREE

/*
 * driver name
 * */
#define SDIO_DRIVER_NAME "test_sdio"

/*
 * File operation structure
 * */
static struct file_operations test_sdio_fops = {
    .owner = THIS_MODULE,
    .open = test_sdio_open,
    .read = test_sdio_read,
    .write = test_sdio_write,
    .unlocked_ioctl = test_sdio_unlocked_ioctl,
};

/*
 * name : test_sdio_probe
 * brief: sdio probe function
 * param: sdio_func: sdio function devices
 * param: sdio_device_id: sdio device id table
 * ret  : sdio driver probe state
 */
int test_sdio_open (struct inode *inode, struct file *file)
{

}

/*
 * name : test_sdio_probe
 * brief: sdio probe function
 * param: sdio_func: sdio function devices
 * param: sdio_device_id: sdio device id table
 * ret  : sdio driver probe state
 */
ssize_t test_sdio_read (struct file *file, char __user *user, size_t, loff_t *loff)
{

}

/*
 * name : test_sdio_probe
 * brief: sdio probe function
 * param: sdio_func: sdio function devices
 * param: sdio_device_id: sdio device id table
 * ret  : sdio driver probe state
 */
ssize_t test_sdio_write (struct file *file, const char __user *user, size_t, loff_t * loff)
{

}
/*
 * name : test_sdio_probe
 * brief: sdio probe function
 * param: sdio_func: sdio function devices
 * param: sdio_device_id: sdio device id table
 * ret  : sdio driver probe state
 */
long test_sdio_unlocked_ioctl (struct file *file, unsigned int cmd, unsigned long argc)
{

}

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
static const struct of_device_id test_sdio_of_table[] {
    { .compatible = "SDIO_DRIVER_NAME" },   // device tree
    {},
};
#endif

/*
 * name : test_sdio_probe
 * brief: sdio probe function
 * param: sdio_func: sdio function devices
 * param: sdio_device_id: sdio device id table
 * ret  : sdio driver probe state
 */
static int test_sdio_probe(struct sdio_func *func, const struct sdio_device_id *id)
{
    int ret = 0;

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

}

/*
 * sdio driver struct
 * */
struct sdio_driver test_sdio_driver {
    .name = "SDIO_DRIVER_NAME",
    .id_atble = test_sdio_id_table,
    .probe = test_sdio_probe,
    .remove = test_sdio_remove,
#ifdef USE_DEVICE_TREE
    .drv = {
        .name = "SDIO_DRIVER_NAME",
        .owner = THIS_MODULE,
        .of_match_table = test_sdio_of_table,
    },
#endif
};

/*
 * name : sdio_init
 * brief: linux module init func
 * param: none
 * ret  : initial state
 */
static int __init sdio_init(void)
{
    int ret = 0;
    pr_info("Init %s driver module.\n", SDIO_DRIVER_NAME);

    /* Request dynamic allocation of a device major number */

    /* Allocate memory for the pre-device structer */

    /* Connect the file operations with the cdev */

    /* Connect the major/minor number to the cdev */

    /* register sdio driver */
    ret = sdio_register_driver(&test_sido_driver);
    if (ret) {
        pr_err("%s: Unable to register as sdio driver.\n", SDIO_DRIVER_NAME);
        goto uable;
    }
    else {
        pr_err("%s: Success register as sdio driver.\n", SDIO_DRIVER_NAME);
        goto exit;
    }

unable:
    pr_err("%s: Driver initial failed.\n", SDIO_DRIVER_NAME);

exit:
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
}

/*
 * module init and exit register
 * */
module_init(sdio_init);
module_exit(sdio_exit);

/*
 * module parameter declare
 * */

MODULE_LICENSE("GPL2");
MODULE_AUTHOR("Macro, makermuyi@gmail.com");
MODULE_DESCRIPTION("Raspbeery pi sdio test");
MODULE_VERSION("0.0.1");

/*------------------------ end -----------------------*/
