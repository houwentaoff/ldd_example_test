/*
 *  Silicon Labs C2 port Linux support for EFM8SB1
 *
 *  Copyright (c) 2019 Joy Chengdu . China. <544088192@qq.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation
 */

#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/c2port.h>

#define GPIO_TO_PIN(gpio) (933 + (gpio))

#define C2D_IO    GPIO_TO_PIN(20)
#define C2CK_IO    GPIO_TO_PIN(21)

static DEFINE_MUTEX(update_lock);

/*
 * C2 port operations
 */

static void c2port_access(struct c2port_device *dev, int status)
{
    mutex_lock(&update_lock);

    /* 0 = input, 1 = output */
    if (status)
    {
        gpio_direction_output(C2D_IO, 1);
        gpio_direction_output(C2CK_IO, 1);
    }
    else
    {
        /* When access is "off" is important that both lines are set
         * as inputs or hi-impedance */
        gpio_direction_input(C2D_IO);
        gpio_direction_input(C2CK_IO);
    }

    mutex_unlock(&update_lock);
}

static void c2port_c2d_dir(struct c2port_device *dev, int dir)
{
    mutex_lock(&update_lock);

    if (dir)
    {
        gpio_direction_input(C2D_IO);
    }
    else
    {
        gpio_direction_output(C2D_IO, 1);
    }

    mutex_unlock(&update_lock);
}

static int c2port_c2d_get(struct c2port_device *dev)
{
    return gpio_get_value(C2D_IO);
}

static void c2port_c2d_set(struct c2port_device *dev, int status)
{
    mutex_lock(&update_lock);

    if (status)
        gpio_set_value(C2D_IO, 1);
    else
        gpio_set_value(C2D_IO, 0);

    mutex_unlock(&update_lock);
}

static void c2port_c2ck_set(struct c2port_device *dev, int status)
{
    mutex_lock(&update_lock);

    if (status)
        gpio_set_value(C2CK_IO, 1);
    else
        gpio_set_value(C2CK_IO, 0);

    mutex_unlock(&update_lock);
}

static struct c2port_ops efm_c2port_ops = {
    .block_size	= 512,	/* bytes */
    .blocks_num	= 16,	/* total flash size: 8K bytes */

    .access		= c2port_access,
    .c2d_dir	= c2port_c2d_dir,
    .c2d_get	= c2port_c2d_get,
    .c2d_set	= c2port_c2d_set,
    .c2ck_set	= c2port_c2ck_set,
};

static struct c2port_device *c2port_dev;

/*
 * Module stuff
 */

static int __init c2port_init(void)
{
    int ret = 0;
    gpio_request(C2D_IO, "c2 data");
    gpio_request(C2CK_IO, "c2 clk");

    c2port_dev = c2port_device_register("uc",
            &efm_c2port_ops, NULL);
    if (NULL == c2port_dev) {
        ret = -1;//PTR_ERR(c2port_dev);
        goto free_region;
    }

    return 0;

free_region:
    gpio_free(C2D_IO);
    gpio_free(C2CK_IO);
    return ret;
}

static void __exit c2port_exit(void)
{
    /* Setup the GPIOs as input by default (access = 0) */
    c2port_access(c2port_dev, 0);

    c2port_device_unregister(c2port_dev);

    gpio_free(C2D_IO);
    gpio_free(C2CK_IO);
}

module_init(c2port_init);
module_exit(c2port_exit);

MODULE_AUTHOR("joy <544088192@qq.com>");
MODULE_DESCRIPTION("Silicon Labs C2 port Linux support for efm8sb1");
MODULE_LICENSE("GPL");
