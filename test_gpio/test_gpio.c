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

/* 1. 使用 mount -t debugfs debugfs /sys/kernel/debug ;
 * cat /sys/kernel/debug/gpio 查看内核中注册的gpio
 * 2. gpio中断最好在内核注册使用完成量(中断可以累加)，不要在应用层使用poll /sys/class/gpio，
 * 因为 应用层会丢中断(poll返回后再次调用Poll需要时间，这段时间是空的).
 */
static irqreturn_t act8945a_status_changed(int irq, void *dev_id)
{
    printk("status changed!");
    return IRQ_HANDLED;
}
static int dm90001_probe(struct platform_device *pdev)
{
    struct gpio_desc *status0_gpio;
    struct gpio_desc *reset_gpio;
    struct gpio_desc *status0_gpio2;
    int ret = 0;
    //对应为设备树名字 :  gpio-status0
    /* gpio suffixes used for ACPI and device tree lookup */                                  
    // static const char * const gpio_suffixes[] = { "gpios", "gpio" };
    // Optional pins ??
    /* 如果gpios中有多个io,可使用devm_gpiod_get_index_optional*/
    /*
     *   如下
         Request GPIOs. 
         for (i = 0; i < state->info->num_dv_ports; ++i) {
             state->hpd_gpio[i] =
                 devm_gpiod_get_index_optional(&client->dev, "hpd", i,
                                   GPIOD_OUT_LOW);
             if (IS_ERR(state->hpd_gpio[i]))
                 return PTR_ERR(state->hpd_gpio[i]);
         
             if (state->hpd_gpio[i])
                 v4l_info(client, "Handling HPD %u GPIO\n", i);
         }
     */
    status0_gpio = devm_gpiod_get_optional(&pdev->dev, "status0", GPIOD_OUT_LOW);
    if (!PTR_ERR_OR_ZERO(status0_gpio))
        gpiod_direction_input(status0_gpio);

    reset_gpio = devm_gpiod_get(&pdev->dev, "reset", GPIOD_OUT_LOW);
    if (!IS_ERR(reset_gpio)) {
        udelay(1);
        ret = gpiod_direction_output(reset_gpio, 1);
    }
    
	status0_gpio2 = devm_gpiod_get_index_optional(&pdev->dev, "status0",
	                        1,
						    GPIOD_IN);
	if (IS_ERR(status0_gpio2)) {
		ret = PTR_ERR(status0_gpio2);
		printk("unable to claim gpio \"lbo\": %d\n", ret);
		goto err;
	}

	ret = devm_request_irq(&pdev->dev, gpiod_to_irq(status0_gpio2),
			       act8945a_status_changed,
			       (IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING),
			       "act8945a_lbo_detect", NULL);
	if (ret){
		printk("unable to request gpio irq \"lbo\": %d\n", ret);
    }
    mdelay(10);
err:    
    return ret;
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

   reset-gpios = <&gpio 1 GPIO_ACTIVE_LOW>;//GPIO_ACTIVE_LOW 可写成0/1
   };
*/
MODULE_AUTHOR("Joy.Hou Chengdu China");
MODULE_DESCRIPTION("test platform driver");
MODULE_LICENSE("GPL");
