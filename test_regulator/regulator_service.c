/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  regulator_service.c
 *
 *    Description:  在没有设备树的情况下的电源调节器的服务端的注册方法
 *                  1. 在enable中添加GPIO这种（若你的电源是GPIO控制enable disable）
 *         Others:
 *
 *        Version:  1.0
 *        Created:  04/20/2019 03:05:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */

/
#include <linux/module.h>
#include <linux/err.h>
#include <linux/export.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>

static struct regulator_dev *dummy_regulator_rdev;

static struct regulator_init_data dummy_initdata = {
    //.supply_regulator = "axxb",//如果有 依赖上级 电源 才会填写该字段（会在enable的时候将依赖上级的电源enable） 电源树
	.constraints = {
        .always_on = 0,
        /* .name --> /sys/class/regulator/regulator.1/name  */
        .name = "aabbccdd",
        .min_uV = 1800000,
        .max_uV = 2000000,
        .valid_ops_mask = REGULATOR_CHANGE_STATUS | REGULATOR_CHANGE_VOLTAGE, // 如果权限不够，enable的操作并不会执行 
        //regulator_ops_is_valid 上面权限检测为该API
        .valid_modes_mask = REGULATOR_MODE_NORMAL,
    },
};

static int is_enabled = 0;

static int ab3100_enable_regulator(struct regulator_dev *reg)
{
    (void)(reg);
    printk("==>%s\n", __func__);
    is_enabled = 1;
    printk("<==%s\n", __func__);
    return 0;
}
static int ab3100_disable_regulator(struct regulator_dev *reg)
{
    (void)(reg);
    printk("==>%s\n", __func__);
    is_enabled = 0;
    printk("<==%s\n", __func__);
    return 0;
}
static int ab3100_is_enabled_regulator(struct regulator_dev *reg)
{
    int enabled = 0;
    (void)(reg);

    printk("==>%s\n", __func__);
    if (is_enabled)
        enabled = 1;
    else 
        enabled = 0;
    printk("<==%s\n", __func__);

    return enabled;
}
static struct regulator_ops dummy_ops = {
//	.list_voltage = regulator_list_voltage_linear,
	.enable      = ab3100_enable_regulator,
	.disable     = ab3100_disable_regulator,
	.is_enabled  = ab3100_is_enabled_regulator,
};

static const struct regulator_desc dummy_desc = {
	.name = "regulator-test",
	.id = 0,
    /* .type --> /sys/class/regulator/regulator.1/type */
	.type = REGULATOR_VOLTAGE,
	.owner = THIS_MODULE,
	.ops = &dummy_ops,
};

static int dummy_regulator_probe(struct platform_device *pdev)
{
	struct regulator_config config = { };
	int ret;

	config.dev = &pdev->dev;
	config.init_data = &dummy_initdata;

	dummy_regulator_rdev = regulator_register(&dummy_desc, &config);
	if (IS_ERR(dummy_regulator_rdev)) {
		ret = PTR_ERR(dummy_regulator_rdev);
		pr_err("Failed to register regulator: %d\n", ret);
		return ret;
	}

	return 0;
}

static struct platform_driver dummy_regulator_driver = {
	.probe		= dummy_regulator_probe,
	.driver		= {
		.name		= "test-reg-dummy",
	},
};

static struct platform_device *dummy_pdev;

static int __init regulator_dummy_init(void)
{
	int ret;

    /*-----------------------------------------------------------------------------
     *  platform device --> /sys/bus/platform/devices/test-reg-dummy 
     *-----------------------------------------------------------------------------*/
	dummy_pdev = platform_device_alloc("test-reg-dummy", -1);
	if (!dummy_pdev) {
		pr_err("Failed to allocate dummy regulator device\n");
		return 0;
	}

	ret = platform_device_add(dummy_pdev);
	if (ret != 0) {
		pr_err("Failed to register dummy regulator device: %d\n", ret);
		platform_device_put(dummy_pdev);
		return 0;
	}

	ret = platform_driver_register(&dummy_regulator_driver);
	if (ret != 0) {
		pr_err("Failed to register dummy regulator driver: %d\n", ret);
		platform_device_unregister(dummy_pdev);
	}
    return 0;
}
module_init(regulator_dummy_init);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Joy Hou <544088192@qq.com>");
MODULE_DESCRIPTION("Driver to test regulator.");
MODULE_VERSION("1.01");
