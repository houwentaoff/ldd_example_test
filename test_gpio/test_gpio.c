/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  test_gpio.c
 *
 *    Description:  gpio   dts
 *         Others:
 *
 *        Version:  1.0
 *        Created:  08/21/2019 10:30:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <linux/of.h>

static int dm90001_probe(struct platform_device *pdev)
{
    struct gpio_desc *status0_gpio;
    struct gpio_desc *reset_gpio;
    int ret = 0;
    //对应为设备树名字 :  gpio-status0
    /* gpio suffixes used for ACPI and device tree lookup */                                  
    // static const char * const gpio_suffixes[] = { "gpios", "gpio" };
    // Optional pins ??

    status0_gpio = devm_gpiod_get_optional(&pdev->dev, "status0", GPIOD_OUT_LOW);
    if (!PTR_ERR_OR_ZERO(status0_gpio))
        gpiod_direction_input(status0_gpio);

    reset_gpio = devm_gpiod_get(&pdev->dev, "reset", GPIOD_OUT_LOW);
    if (!IS_ERR(reset_gpio)) {
        udelay(1);
        ret = gpiod_direction_output(reset_gpio, 1);
    }

    mdelay(10);
    return 0;
}
static int dm9000_drv_remove(struct platform_device *pdev)
{
    return 0;
}
static const struct of_device_id mt7620_pci_ids[] = {
    { .compatible = "mediatek,mt7620-pci" },
    {.compatible = "mediatek,aabbcc" },
    {}  
};       

static struct platform_driver test_platform_driver = {
    .driver= {
        .name  = "test platform",
        .owner = THIS_MODULE,
        .of_match_table    = of_match_ptr(mt7620_pci_ids),
    },
    .probe   = dm90001_probe,
    .remove  = dm9000_drv_remove,
};

module_platform_driver(test_platform_driver);
/*
   foo_device {
   compatible = "mediatek,aabbcc";
   ...
   status0-gpios = <&gpio 15 GPIO_ACTIVE_HIGH>, // red 
   <&gpio 16 GPIO_ACTIVE_HIGH>, // green
   <&gpio 17 GPIO_ACTIVE_HIGH>; // blue 

   reset-gpios = <&gpio 1 GPIO_ACTIVE_LOW>;
   };
*/
MODULE_AUTHOR("Joy.Hou Chengdu China");
MODULE_DESCRIPTION("test platform driver");
MODULE_LICENSE("GPL");
