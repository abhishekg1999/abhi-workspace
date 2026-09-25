#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

static int __init hello_init(void)
{
    pr_info("Hello World: module loaded\n");
    return 0;
}

static void __exit hello_exit(void)
{
    pr_info("Hello World: module unloaded\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VVDN");
MODULE_DESCRIPTION("Simple Hello World kernel module");
MODULE_VERSION("1.0");
