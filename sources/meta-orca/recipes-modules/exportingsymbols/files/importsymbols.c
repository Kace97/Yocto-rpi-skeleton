#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

extern void print_export_string(void);
extern int JUST_AN_INT;
extern void func_with_args(int, int);

static int __init import_init(void)
{
    pr_info("this is the module which will import symbols.\n");
    pr_info("the imported int was %d.\n", JUST_AN_INT);
    print_export_string();
    func_with_args(1, 15);
    return 0;
}

static void __exit import_exit(void)
{
    pr_info("alright thats all. Leaving the import driver.\n");
}

module_init(import_init);
module_exit(import_exit);


MODULE_AUTHOR("BIG DAVE");
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL");
