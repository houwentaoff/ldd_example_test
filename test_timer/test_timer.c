/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  test_platform1.c
 *
 *    Description:  
 *                  
 *         Others:  
 *                  platform_driver_register platform_driver_unregister
 *                  platform_get_drvdata platform_set_drvdata
 *
 *        Version:  1.0
 *        Created:  Sunday, August 14, 2016 10:48:35 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */

#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joy.Hou");
static struct timer_list my_timer;
static struct class *timer_class;
static struct device *test_dev1;
unsigned int timeperiod = 0;
//static struct device *test_dev2;

/*
 * The "foo" file where a static variable is read from and written to.
 */
static ssize_t on_show(struct kobject *kobj, struct kobj_attribute *attr,
    char *buf)
{
      return sprintf(buf, "%s\n", timer_pending(&my_timer) ? "true" : "false");
}

static ssize_t on_store(struct kobject *kobj, struct kobj_attribute *attr,
      const char *buf, size_t count)
{
    unsigned int flag = 0;
    sscanf(buf, "%u", &flag);
    switch (flag)
    {
        case 1:
            if (timeperiod != 0)
            {
                mod_timer(&my_timer, jiffies + timeperiod);
            }
            break;
        case 0:
            if (timer_pending(&my_timer))
            {
                del_timer_sync(&my_timer);
            }
            break;
        default:
            break;
    }
    return count;
}
static ssize_t value_store(struct kobject *kobj, struct kobj_attribute *attr,
      const char *buf, size_t count)
{
    unsigned int time = 0;
    sscanf(buf, "%u", &time);
    printk("time = %ums.\n", time);
    time = time *HZ/1000;
    timeperiod = time;
    printk("jiffies = %u/s.\n", time);
    mod_timer(&my_timer, jiffies + time);
    printk("finish.\n");
    return count;
}

//static struct device_attribute foo_attribute =
//      __ATTR(value, 0666, foo_show, foo_store);
static DEVICE_ATTR(value, S_IWUSR, NULL, value_store);

static DEVICE_ATTR(on, S_IWUSR|S_IRUSR , on_show, on_store);

/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *attrs[] = {
      &dev_attr_value.attr,
      &dev_attr_on.attr,
      NULL, /* need to NULL terminate the list of attributes */
};

/*
 * An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.
 */
static struct attribute_group attr_group = {
      .attrs = attrs,
};
static void timer_func(struct timer_list * t)
{
    printk("b\n");

//    timer_setup(&my_timer, timer_func, 0);
//    mod_timer(&my_timer, jiffies + 1*HZ/100);
    my_timer.expires = jiffies + timeperiod;
//    my_timer.function = timer_func;
    add_timer(&my_timer);
}
#if 0
static void start_timer(void)
{
    timer_setup(&my_timer, timer_func, 0);
//    my_timer.expires = jiffies + 1*HZ/100;
//    add_timer(&my_timer);
}
#endif
static int __init example_init(void)
{
    int retval = 0;

    timer_class   = class_create(THIS_MODULE, "timer");
    test_dev1   = device_create(timer_class, NULL, 0, NULL, "timer1");//timer1/目录下创建xxx目录
    retval = sysfs_create_group(&test_dev1->kobj, &attr_group);
    if (retval)
    {
        printk("[test sysfs]: sysfs create group fail.\n");
    }
    timer_setup(&my_timer, timer_func, 0);

    return retval;
}

static void __exit example_exit(void)
{
    printk("==> %s\n", __func__);
    if (timer_pending(&my_timer))
    {
        del_timer_sync(&my_timer);
    }
    device_del(test_dev1);
    if (timer_class)
    {
        class_destroy(timer_class);
    }
    printk("<== %s\n", __func__);
    printk("demo exit\n");
}

module_init(example_init);
module_exit(example_exit);

//MODULE_DESCRIPTION("test timer");
