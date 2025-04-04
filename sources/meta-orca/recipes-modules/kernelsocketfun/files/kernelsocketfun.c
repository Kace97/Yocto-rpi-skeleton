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
#include "ksf.h"


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

static const struct nla_policy ksf_get_secret_message_policy[KSF_ATTR_MAX] = {
    [KSF_MESSAGE_SELECT] = {.type = NLA_U8}
};

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
    {   .cmd = KSF_GET_SECRET_MESSAGE,
        .policy = ksf_get_secret_message_policy,
        .doit = ksf_get_secret_message,
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

static int ksf_hello(struct sk_buff *skb, struct genl_info *info)
{
    pr_info("hello, I'm just a mere example command\n");
    return 0;
}

static int ksf_log_message(struct sk_buff *skb, struct genl_info *info) {
    char* log_message;

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

static int ksf_get_secret_message(struct sk_buff *skb, struct genl_info *info) {
    void* hdr;
    int message_select;
    struct nlattr **attrs = info->attrs;
    char* carSecret = "The car has no wheels...";
    char* computerSecret = "The computer has no memory....";
    struct sk_buff *msg;

    if(IS_ERR(hdr))
    {
        pr_err("Failed to create message header.\n");
        return PTR_ERR(hdr);
    }

    if (!info->attrs[KSF_MESSAGE_SELECT])
    {
        pr_warn("Message didn't have all required attributes.\n");
        return -1;
    }

    msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
    if (!msg)
    {
        pr_err("No memory for allocating new kernel message.\n");
        return -ENOMEM;
    }
    hdr = genlmsg_put(msg, info->snd_portid, info->snd_seq, &ksf_fam, 0, KSF_MESSAGE_GET);
    if(!hdr)
    {
        pr_err("Failed to create genl header.\n");
        return -ENOBUFS;
    }

    message_select = nla_get_u8(attrs[KSF_MESSAGE_SELECT]);
    switch (message_select)
    {
        case KSF_CAR_SECRET: {
            pr_info("Car Secret Selected\n");
            nla_put_string(msg, KSF_SECRET_MESSAGE, carSecret);
            break;
        }
        case KSF_COMPUTER_SECRET: {
            pr_info("Computer Secret Selected\n");
            nla_put_string(msg, KSF_SECRET_MESSAGE, computerSecret);
            break;
        }
        case KSF_MESSAGE_UNDEF: {
            pr_err("Undefined value, perhaps the kernelsocketfun module is out of date.");
            break;
        }
        default: {
            pr_err("Unrecognized value");
            break;
        }
    }

    pr_info("putting the bow on top of our genlmsg.\n");
    genlmsg_end(msg, hdr);
    pr_info("sending the response back up\n");
    return genlmsg_reply(msg, info);
}

static int __init ksf_initialize(void) {
    int err;
    pr_info("initializing netlink family\n");
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
