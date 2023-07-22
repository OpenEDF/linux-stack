/*--------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/uaccess.h>

/** Log entry point for debugging */
#define ENTER() pr_info("--->: %s\n", __func__)
/** Log exit point for debugging */
#define LEAVE() pr_info("<---: %s\n", __func__)

static u8 *buf;
#define LINE_SIZE 16

/* ------------- read files ---------------- */
static void sample_fw_load(struct file *filp, loff_t size)
{
    u8 index = 0x00;
    u16 addr = 0x00;
    loff_t rd_pos = 0x00;
    loff_t wr_pos = size + 32;
    loff_t size_a;
    char *data = "macro_linux_test";
    loff_t buf_size = size + 32;

    ENTER();
    buf = kmalloc(buf_size + 1, GFP_KERNEL);
    memset(buf, 0x00, buf_size + 1);
    buf[buf_size + 1] = '\0';

    /* write */
    kernel_write(filp, data, strlen(data), &wr_pos);
    size_a = i_size_read(file_inode(filp));
    pr_info("after wirte file size: %lld\n", size_a);
    pr_info("write buf curr addr is: %lld byte", wr_pos);

    /* read */
    kernel_read(filp, buf, 64, &rd_pos);
    pr_info("read buf curr addr is: %lld byte", rd_pos);

    /* dump */
    pr_info("------------------------ dump bin -------------------------\r\n");
    for (addr = 0x00; addr < (buf_size / LINE_SIZE); addr++) {
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
    struct file *filp;
    loff_t size;
    mm_segment_t old_fs;
    old_fs = force_uaccess_begin();
    ENTER();

    /* Linux read file test */
    printk(KERN_INFO "linux read file test...\n");

    /* use the default method to get the firmware */
    filp = filp_open("sample_firmware.bin", O_RDWR | O_CREAT, 0644);
    if (IS_ERR(filp) || (filp == NULL)) {
        pr_info("read file error!\r\n");
        return -1;
    }
    size = i_size_read(file_inode(filp));
    pr_info("file size: %lld\n", size);
    sample_fw_load(filp, size);

    filp_close(filp, NULL);
    force_uaccess_end(old_fs);
    pr_info("module init done!\n");
    LEAVE();
    return 0;
}

static void __exit mod_exit(void)
{
    ENTER();
    printk(KERN_INFO "linux read file test exit!\n");
    LEAVE();
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

/*--------------------------------------------------------------------*/
