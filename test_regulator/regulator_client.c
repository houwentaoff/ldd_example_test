/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  regulator_client.c
 *
 *    Description:  
 *         Others: 若编译提示未定义警告 其中api内核是否有包含, 未实现在nconfig中选中并重新编译
 *                 在qfpuse的样例中使用过 新版内核已经将高通qfpuse文件删除.
 *
 *        Version:  1.0
 *        Created:  04/20/2019 10:50:36 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */

/*-----------------------------------------------------------------------------
 *  参考文档 Documentation/power/regulator
 *  consumer.txt  design.txt  machine.txt  overview.txt  regulator.txt
 *-----------------------------------------------------------------------------*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/regulator/consumer.h>
#include <linux/platform_device.h>

#define PMXXX_REGULATOR  "aabbccdd"            /*  */

static struct regulator * digital =  NULL;

static int qfp_fuse_probe(struct platform_device *pdev)
{
    int err = 0;
    (void)(pdev);

    /*-----------------------------------------------------------------------------
     * 如没有找到会使用系统虚假的 会打印如下
     *  (NULL device *): deviceless supply regulator-test not found, using dummy regulator
     *-----------------------------------------------------------------------------*/
    digital = regulator_get(NULL, PMXXX_REGULATOR);
    if (!digital) {
        pr_err("[%s] Error getting %s regulator\n",
                __func__, PMXXX_REGULATOR);
        return PTR_ERR(digital);
    }
//    printk("enable regulator[%s]\n", digital->supply_name);
    printk("enable regulator ...\n");
    err = regulator_enable(digital);
    if (err != 0) {
        pr_err(
                "[%s] Error enabling PMXXX regulator err[%d]\n", __func__, err);
        return -EFAULT;
    }
    mdelay(1);
    printk("disable regulator ...\n");
    err = regulator_disable(digital);
    if (err != 0) {
        pr_err(
                "[%s] Error disabling PMxxx regulator err[%d]\n", __func__, err);
        return -EFAULT;
    }
    return 0;
}
static int  qfp_fuse_remove(struct platform_device *plat)
{
    pr_info("[%s]\n", __func__);
    if (digital)
        regulator_put(digital);
    return 0;
}

static int __init qfp_fuse_init(void)
{
    return qfp_fuse_probe(NULL);
}

static void __exit qfp_fuse_exit(void)
{
    qfp_fuse_remove(NULL);
}

module_init(qfp_fuse_init);
module_exit(qfp_fuse_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Joy Hou <544088192@qq.com>");
MODULE_DESCRIPTION("Driver to test regulator.");
MODULE_VERSION("1.01");
