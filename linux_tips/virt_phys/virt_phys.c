#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/moduleparam.h>
#include <asm/io.h>
#include <linux/kernel.h>

static void *virtual_addr;
static phys_addr_t physical_addr;
int size = 1024; /* allocate 1024 size */

static int __init mod_int(void)
{
    virtual_addr = kmalloc(size, GFP_ATOMIC);
    if (!virtual_addr) {
        printk(KERN_ERR "memory allocation failed\n");
        return -ENOMEM;
    } else {
        printk(KERN_INFO "memory allcated successfully vitual address: %p\n", virtual_addr);
        physical_addr = virt_to_phys(virtual_addr);
        printk(KERN_INFO "physical address: 0x%x\n", physical_addr);
        printk(KERN_INFO "vitual address: %p\n", virtual_addr);
    }
    return 0;
}

static void __exit mod_exit(void)
{
    kfree(virtual_addr); 
    printk(KERN_INFO "VIRTUAL_TO_PHYSICAL VICE VERSA: GoodBye.\n");
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("CPL");
MODULE_VERSION("1.0");
