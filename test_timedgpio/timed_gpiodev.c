/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  test_platform1.c
 *
 *    Description:  timed gpio的platform 设备, 也可以在dts中构建
 *                  
 *         Others:  定时设置批量GPIO的高低电平
 *                  
 *                  
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
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include "timed_gpio.h"
/**
 * @brief platform device的私有数据
 */
/*  
struct test_data {
    const char * const str;
};
struct test_data test_platform_data = {
    .str  = "this is test platform data!"
};
*/
/**
 * @brief 不加会导致rmsmod时候内核报错
 *
 * @param dev
 */
/*  
static void platform_test_release(struct device * dev)
{
    return ;
}
struct platform_device test_device = {
    .name  = "test platform",
    .dev   = {
        .release       = platform_test_release,
        .platform_data = &test_platform_data,
    }
};
*/
static struct timed_gpio tgpios [] = 
{
    {
       .name = "vibrator",
       .gpio = 7, //对应平台的GPIO号
       .max_timeout = 100000000,
       .active_low = 0,
    },
    {
       .name = "vibrator",
       .gpio = 8, //对应平台的GPIO号
       .max_timeout = 100000000,
       .active_low = 0,
    },
};

static  struct timed_gpio_platform_data timed_gpio_data = 

{
       .num_gpios = 2,
       .gpios = tgpios,//gpios -> *

};

static struct platform_device timed_gpio_dev = 
{
       .name = "timed-gpio",
       .id = -1,
       .dev = 
       {
             .platform_data = &timed_gpio_data,
       },
};

static int __init test_platform_init(void)
{
    printk(KERN_INFO "==>%s init\n", __func__);
    return platform_device_register(&timed_gpio_dev);
}

static void __exit test_platform_exit(void)
{
    platform_device_unregister(&timed_gpio_dev);
    printk(KERN_INFO "<==%s exit\n", __func__);
}

module_init(test_platform_init);
module_exit(test_platform_exit);
MODULE_AUTHOR("Joy.Hou China Chengdu");
MODULE_DESCRIPTION("test platform driver");
MODULE_LICENSE("GPL");
