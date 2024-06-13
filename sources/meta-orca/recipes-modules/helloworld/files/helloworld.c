#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init hello_init(void) {
    pr_info("starting up the easiest module of all time.\n");
    return 0;
}

static void __exit hello_exit(void) {
    pr_info("closing down the easiest module of all time.\n");
}


module_init(hello_init);
module_exit(hello_exit);


MODULE_AUTHOR("BIG DAVE");
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL");
