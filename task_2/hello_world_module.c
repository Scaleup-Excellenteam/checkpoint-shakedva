#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shaked");
MODULE_DESCRIPTION("Example for kernel module");
MODULE_VERSION("0.01");

static int __init lkm_example_init(void) {
    printk(KERN_INFO "hello from checkpoint\n");
    return 0;
}

static void __exit lkm_example_exit(void) {
    printk(KERN_INFO "bye from checkpoint\n");
}


module_init(lkm_example_init);
module_exit(lkm_example_exit);
