#include <linux/module.h>
#include <linux/kernel.h>

int JUST_AN_INT = 5;
EXPORT_SYMBOL(JUST_AN_INT);

void print_export_string(void)
{
    pr_info("This is proof that a module was exported\n");
}
EXPORT_SYMBOL(print_export_string);

void func_with_args(int a, int b)
{
    pr_info("the first string was %d.\n the second was %d.\n", a, b);
}
EXPORT_SYMBOL(func_with_args);


MODULE_AUTHOR("BIG DAVE");
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL");
