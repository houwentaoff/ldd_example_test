/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  test_sysfs.c
 *
 *    Description:  测试在sysfs/class下创建soc_adc/adc1/value soc_adc/adc2/value文件，
 *                  使用device_del能正常卸载并重复加载由于没有dev_t不能使用device_destroy
 *         Others:
 *
 *        Version:  1.0
 *        Created:  Wednesday, August 10, 2016 11:02:24 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */

#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/io.h>

/*
 * This module shows how to create a simple subdirectory in sysfs called
 * /sys/kernel/kobject-example  In that directory, 3 files are created:
 * "foo", "baz", and "bar".  If an integer is written to these files, it can be
 * later read out of it.
 */

static int foo = 0;
static struct class *soc_adc_class;
static struct device *test_dev1;
static struct device *test_dev2;
/*
 * The "foo" file where a static variable is read from and written to.
 */


static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
    char *buf)
{
      return sprintf(buf, "%d\n", foo);
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
      const char *buf, size_t count)
{
     sscanf(buf, "%du", &foo);
     printk("foo = %d.\n",foo);

     foo = foo -1;

     if  (foo < 0 || foo > 1)
     {
           printk("input foo error. foo=%d.\n",foo);
           return 0;
     }
      
     printk("finish.\n");
     return count;
}

//static struct device_attribute foo_attribute =
//      __ATTR(value, 0666, foo_show, foo_store);
static DEVICE_ATTR(value, S_IWUSR | S_IRUGO, foo_show, foo_store);

/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *attrs[] = {
      &dev_attr_value.attr,
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

static int __init example_init(void)
{
     int retval = 0;

     soc_adc_class   = class_create(THIS_MODULE, "soc_adc");
     test_dev1   = device_create(soc_adc_class, NULL, 0, NULL, "adc1");//sec/目录下创建xxx目录
     test_dev2   = device_create(soc_adc_class, NULL, 0, NULL, "adc2");//sec/目录下创建xxx目录
     retval = sysfs_create_group(&test_dev1->kobj, &attr_group);
     if (retval)
     {
        printk("[test sysfs]: sysfs create group fail.\n");
     }
     retval = sysfs_create_group(&test_dev2->kobj, &attr_group);
     if (retval)
     {
        printk("[test sysfs]: sysfs create group fail.\n");
     }

     return retval;
}

static void __exit example_exit(void)
{
    printk("==> %s\n", __func__);
    device_del(test_dev1);
    device_del(test_dev2);
    if (soc_adc_class)
    {
        class_destroy(soc_adc_class);
    }
    printk("<== %s\n", __func__);
    printk("demo exit\n");
}

module_init(example_init);
module_exit(example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joy.Hou");
