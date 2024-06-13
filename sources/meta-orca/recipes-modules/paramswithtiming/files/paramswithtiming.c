// just general linux headers that are either required or good to have
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

// this one is for the sleep
#include <linux/delay.h>

// these are for the work queue
#include <linux/workqueue.h>
#include <linux/sched.h>

#define pr_fmt(fmt) "paramsWithTiming:%s: " fmt, __func__

static int secondsPerMsg = 1;
module_param(secondsPerMsg, int, S_IRUGO);
MODULE_PARM_DESC(secondsPerMsg, "sets how many seconds between printing the message for this module");

static struct work_struct print_wrk; // saw this in a book but haven't ready thing on it yet but it seems like what I want.

static void print_handler(struct work_struct *work)
{
    while(1)
    {
        msleep(1000 * secondsPerMsg); // unlike tasklets work queue's can sleep
        pr_info("It has been %d seconds so heres your message.\n", secondsPerMsg);
    }
}

static int __init paramsWithTiming_init(void)
{
    pr_info("I will print a message every %d seconds\n",secondsPerMsg);
    INIT_WORK(&print_wrk, print_handler); // the work_struct acts as a work queue entry
    schedule_work(&print_wrk); // here we actually can put our work into the queue
    return 0;
}

static void __exit paramsWithTiming_exit(void)
{
    pr_info("Closing down the paramsWithTiming module.\n");
}

module_init(paramsWithTiming_init);
module_exit(paramsWithTiming_exit);

MODULE_AUTHOR("BIG DAVE");
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL");
