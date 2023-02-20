#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/of.h>

#define ROOT_NODE NULL
typedef struct {
    struct device_node *dev_nd;
}dts_of_t;

static dts_of_t dts_of;

static int __init mod_int(void)
{
    dts_of_t *dev_tmp = &dts_of;
    /* Linux device tree node find test */
    printk(KERN_INFO "Linux device tree node test:\n");
    dev_tmp->dev_nd = of_find_node_by_name(ROOT_NODE, "leds");
    if (dev_tmp->dev_nd) {
        printk(KERN_INFO "Linux find the node name is: %s.\n", dev_tmp->dev_nd->name);
        printk(KERN_INFO "Linux find the node full name is: %s.\n", dev_tmp->dev_nd->full_name);
        printk(KERN_INFO "Linux find the node property is: %s.\n", dev_tmp->dev_nd->properties->value);
    } else {
        printk(KERN_INFO "Linux not find the node.\n");
    }
    return 0;
}

static void __exit mod_exit(void)
{
    printk(KERN_INFO "Linux device tree node test exit\n");
}

module_init(mod_int);
module_exit(mod_exit);
MODULE_LICENSE("CPL");
MODULE_VERSION("1.0");
