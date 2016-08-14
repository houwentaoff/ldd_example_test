/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  test_platform1.c
 *
 *    Description:  测试在bsp中和在当前驱动中直接注册platform device
 *                  2中platform方法
 *         Others:  相关的api:
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
#if 0

/*-----------------------------------------------------------------------------
 *  bsp中的定义如下
 *-----------------------------------------------------------------------------*/
static struct platform_device *smdkc210_devices[] __initdata = {
    &s3c_device_hsmmc0,
    &s3c_device_hsmmc1,
    &s3c_device_hsmmc2,
    &s3c_device_hsmmc3,
    &s3c_device_i2c1,
    &s3c_device_rtc,
    &s3c_device_wdt,
    &exynos4_device_ac97,
    &exynos4_device_i2s0,
    &exynos4_device_pd[PD_MFC],
    &exynos4_device_pd[PD_G3D],
    &exynos4_device_pd[PD_LCD0],
    &exynos4_device_pd[PD_LCD1],
    &exynos4_device_pd[PD_CAM],
    &exynos4_device_pd[PD_TV],
    &exynos4_device_pd[PD_GPS],
    &exynos4_device_sysmmu,
    &samsung_asoc_dma,
    &smdkc210_smsc911x,
};
/*  */
platform_add_devices(smdkc210_devices, ARRAY_SIZE(smdkc210_devices));
#endif

#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

/**
 * @brief platform device的私有数据
 */
struct test_data {
    const char * const str;
};
struct test_data test_platform_data = {
    .str  = "this is test platform data!"
};
/**
 * @brief 不加会导致rmsmod时候内核报错
 *
 * @param dev
 */
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

static int dm9000_drv_remove(struct platform_device *pdev)
{
    struct test_data *pdata = pdev->dev.platform_data;// platform的void data
    char *rbuff = platform_get_drvdata(pdev);

    printk("==>%s data[%s]\n", __func__, pdata->str);
    
    printk("<==%s rbuff[%s]\n", __func__, rbuff);
    if (rbuff) {
        kfree(rbuff);
    }
    platform_set_drvdata(pdev, NULL);
    return 0;
}
static int dm90001_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct test_data *pdata = pdev->dev.platform_data;
    char * rbuff = NULL;

    printk("==>%s data[%s]\n", __func__, pdata->str);

    rbuff = kmalloc(100, GFP_KERNEL);
    if (rbuff == NULL ) {
        ret = -ENOBUFS;
        goto out_free;
    }
    platform_set_drvdata(pdev, rbuff);
    strcpy(rbuff, "just test set/get drv data!!");

    printk("<==%s data[%s]rbuff[%s]\n", __func__, pdata->str,
            rbuff);

    return ret;
out_free:
    kfree(rbuff);    
    return ret;
}
static struct platform_driver test_platform_driver = {
    .driver= {
        .name  = "test platform",
        .owner = THIS_MODULE,
//        .pm    = &dm9000_drv_pm_ops,
    },
    .probe   = dm90001_probe,
    .remove  = dm9000_drv_remove,
};
static int test_platform_device_init(void)
{
    return platform_device_register(&test_device);
}
static void test_platform_device_exit(void)
{
    platform_device_unregister(&test_device);
}
static int __init test_platform_init(void)
{
    printk(KERN_INFO "==>%s init\n", __func__);
    test_platform_device_init();
    return platform_driver_register(&test_platform_driver);
}

static void __exit test_platform_exit(void)
{
    test_platform_device_exit();
    platform_driver_unregister(&test_platform_driver);
    printk(KERN_INFO "<==%s exit\n", __func__);
}

module_init(test_platform_init);
module_exit(test_platform_exit);
MODULE_AUTHOR("Joy.Hou China Chengdu");
MODULE_DESCRIPTION("test platform driver");
MODULE_LICENSE("GPL");
