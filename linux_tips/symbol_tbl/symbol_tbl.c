/*--------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kallsyms.h>

static int prsyms_print_symbol(void *data, const char *namebuf,
                               struct module *module, unsigned long address)
{
    pr_info("### %lx\t%s\n", address, namebuf);
    return 0;
}

static int __init prsyms_init(void)
{
    kallsyms_on_each_symbol(prsyms_print_symbol, NULL);
    return 0;
}

static void __exit prsyms_exit(void)
{
}

module_init(prsyms_init);
module_exit(prsyms_exit);

MODULE_AUTHOR("Sam Protsenko");
MODULE_DESCRIPTION("Module for printing all kernel symbols");
MODULE_LICENSE("GPL");
/*--------------------------------------------------------------------*/
