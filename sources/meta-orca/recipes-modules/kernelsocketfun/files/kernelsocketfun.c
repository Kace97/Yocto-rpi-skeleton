// General includes
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/err.h>

// Netlink includes
#include <linux/netlink.h>
// #include <linux/rtnetlink.h>
#include <linux/genetlink.h>
#include <net/genetlink.h>
#include <net/sock.h>

// My includes
#include "kernelsocketfun.h"


// This gets called before any of the 'doit' operations defined inside of our ops
// is a good place if we have a common mutex that needs to be locked we can do that here
static int ksf_predoit(const struct genl_ops *ksf_ops,
                    struct sk_buff *skb,
                    struct genl_info *info)
{
    pr_info("received a msg over the buffer\n");
    return 0;
}

static void ksf_postdoit(const struct genl_ops *ops,
                        struct sk_buff *skb,
                        struct genl_info *info)
{
    pr_info("we ran a command woohoo\n");
}

static int ksf_hello(struct sk_buff *skb, struct genl_info *info)
{
    pr_info("hello, I'm just a mere example command\n");
    return 0;
}

// the ops array is how we define the API to be leveraged by the userspace
static const struct genl_ops ksf_ops[] = {
    {
        .cmd = KSF_CMD_HELLO,
        .doit = ksf_hello,
    },
};

// here is where we will make our genl family
static struct genl_family ksf_fam = {
    .name = KSF_GENL_NAME, // This should have us generate and assign a unique id for this family automatically
    .hdrsize = 0,
    .n_ops = ARRAY_SIZE(ksf_ops),
    .maxattr = KSF_ATTR_MAX,
    .ops = ksf_ops,
    .pre_doit = ksf_predoit,
    .post_doit = ksf_postdoit,
    .module = THIS_MODULE, // Seems like this would basically always be true
};

static int __init ksf_initialize(void) {
    pr_info("initializing netlink family\n");
    int err = genl_register_family(&ksf_fam);
    if (err)
        return err;
    return 0;
}

static void __exit ksf_exit(void) {
    pr_info("unregistering netlink family\n");
    genl_unregister_family(&ksf_fam);
}

module_init(ksf_initialize);
module_exit(ksf_exit);

MODULE_AUTHOR("BIG DAVE");
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL");
