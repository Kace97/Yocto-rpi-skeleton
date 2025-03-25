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

static const struct nla_policy ksf_log_message_policy[KSF_ATTR_MAX] = {
    [KSF_KERNEL_LOG_MESSAGE] = {.type = NLA_NUL_STRING},
};

static int ksf_hello(struct sk_buff *skb, struct genl_info *info)
{
    pr_info("hello, I'm just a mere example command\n");
    return 0;
}

static int ksf_log_message(struct sk_buff *skb, struct genl_info *info) {
    char* log_message;
    void* hdr;
    struct sk_buff* out_skb;
    int err;

    if(!info->attrs[KSF_KERNEL_LOG_MESSAGE])
    {
        pr_warn("Message didn't have all required attributes.\n");
        return -1;
    }

    log_message = (char*)nla_data(info->attrs[KSF_KERNEL_LOG_MESSAGE]);

    pr_info("Hello our lil module has received a request to log a message.");
    pr_info("mesage to be logged is: %s", log_message);

    return 0;
}

// static const struct nla_policy ksf_dump_secret_message_policy[KSF_ATTR_MAX] = {
//     [KSF_MESSAGE_SELECT] = {.type = NLA_U8}
// };

// the ops array is how we define the API to be leveraged by the userspace
static const struct genl_ops ksf_ops[] = {
    {
        .cmd = KSF_CMD_HELLO,
        .doit = ksf_hello,
    },
    {
        .cmd = KSF_LOG_MESSAGE,
        .policy = ksf_log_message_policy,
        .doit = ksf_log_message,
    },
    // {   .cmd = KSF_GET_SECRET_MESSAGE,
    //     .policy = ksf_dump_secret_message_policy,
    //     // .doit = ,
    //     .dumpit = ksf_dump_secret_message,
    // },
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

// static int ksf_dump_secret_message(struct sk_buff *skb, struct genl_info *info) {
//     void* hdr;

//     char* carSecret = "The car has no wheels...";
//     char* computerSecret = "The computer has no memory....";

//     hdr = genlmsg_put(skb, info->snd_portid, info->snd_seq, &ksf_fam, 0, KSF_MESSAGE_DUMP);
//     if(IS_ERR(hdr))
//     {
//         pr_err("Failed to create message header.\n");
//         return PTR_ERR(hdr);
//     }

//     if (!info->attrs[KSF_MESSAGE_SELECT])
//     {
//         pr_warn("Message didn't have all required attributes.\n");
//         return -1;
//     }


//     if(info->attrs[KSF_MESSAGE_SELECT] == KSF_COMPUTER_SECRET)
//     {
//         pr_info("Computer Secret Selected\n");
//         NLA_PUT_STRING(skb, KSF_SECRET_MESSAGE, computerSecret);
//     }
//     else
//     {
//         pr_info("Car Secret Selected\n");
//         NLA_PUT_STRING(skb, KSF_SECRET_MESSAGE, carSecret);
//     }
//     return genlmsg_reply(skb, info)
// }

// static struct genl_multicast_group ksf_multicast_group = {
//     .name = KSF_GENL_GROUP
// }

static int __init ksf_initialize(void) {
    pr_info("initializing netlink family\n");
    int err;
    err = genl_register_family(&ksf_fam);
    if (err)
    {
        pr_err("Failed to initialize ksf_fam.\n");
        return err;
    }
    pr_info("ksf_fam initialized.\n");
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
