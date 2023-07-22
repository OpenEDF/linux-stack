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
#include <linux/firmware.h>

/* ------------- read files ---------------- */
#define LINE_SIZE 16
static u8 *buf = NULL;
const struct firmware *fw_entry;
struct device dev;

/** Log entry point for debugging */
#define ENTER() pr_info("--->: %s\n", __func__)
/** Log exit point for debugging */
#define LEAVE() pr_info("<---: %s\n", __func__)

static void sample_fw_load(const u8 *firmware, int size)
{
    u8 index = 0x00;
    u16 addr = 0x00;

    ENTER();
    buf = kmalloc(size + 1, GFP_KERNEL);
    memcpy(buf, firmware, size);
    buf[size] = '\0';
    pr_info("------------------------ dump bin -------------------------\r\n");
    for (addr = 0x00; addr < (size / LINE_SIZE); addr++) {
        pr_cont("0x%08x: ", addr * LINE_SIZE);
        for (index = 0x00; index < LINE_SIZE; index++) {
            pr_cont("%02x ", buf[index + (addr * LINE_SIZE)]);
        }
        pr_cont("\r\n");
    }
    pr_info("--------------------------- end ---------------------------\r\n");
    kfree(buf);
    LEAVE();
}

static int __init mod_int(void)
{
    ENTER();
    /* Linux firmware download test */
    printk(KERN_INFO "linux firmware file test...\n");

    /* use the default method to get the firmware */
    if (request_firmware(&fw_entry, "sample_firmware.bin", &dev) != 0x00) {
        pr_info("firmware example: firmware file not file!\r\n");
        return -1;
    }

    pr_info("firmware size: %d byte\n", fw_entry->size);
    sample_fw_load(fw_entry->data, fw_entry->size);
    release_firmware(fw_entry);
    pr_info("module init done!\n");
    LEAVE();
    return 0;
}

static void __exit mod_exit(void)
{
    ENTER();
    printk(KERN_INFO "linux firamware file test exit!\n");
    LEAVE();
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

/*--------------------------------------------------------------------*/
