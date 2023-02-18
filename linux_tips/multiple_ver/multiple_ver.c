#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <generated/utsrelease.h>
#include <linux/version.h>

extern struct module __this_module;

static int __init mod_int(void)
{
    /* Now depends on our kernel version we could log a different comment in the kernel log */ 
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 10) 
    printk(KERN_INFO "KERNEL_VERSION: Hello old kernel %s\n", UTS_RELEASE);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(6, 10, 0)
    printk(KERN_INFO "KERNEL_VERSION: Hello new kernel %s\n", UTS_RELEASE);
#else
    printk(KERN_INFO "KERNEL_VERSION: Hello moderate kernel %s\n", UTS_RELEASE);
#endif
    return 0;
}

static void __exit mod_exit(void)
{
    printk(KERN_INFO "KERNELVERSION: GoodBye.\n");
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("CPL");
MODULE_VERSION("1.0");
