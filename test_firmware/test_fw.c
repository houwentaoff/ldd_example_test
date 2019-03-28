/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  test_fw.c
 *
 *    Description:  1. 很多固件可能是非二进制，比如很多单片机是ihex文本形式,这时需要内核中的
 *                  ihex2fw app测试代码先做转换到2进制然后进行对ihex2fw中链表进行下载到单
 *                  片机中
 *                  2. 不论什么格式需要自己在内核中对映射进来的fw->data进行转换然后烧写到单片
 *                  机中
 *                  3. 涉及到烧写时序参考c2 interface ,c2port中使用的local_irq_disable() local_irq_enable()
 *                  4. fw的默认路径查看内核源码可以看到大致为'/lib/firmware ....'
 *         Others:
 *
 *        Version:  1.0
 *        Created:  03/26/2019 10:17:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/firmware.h>

/**
 * @brief in the function ,you can use spi/c2/i2c ....interface to transmit fw data to device.
 *
 * @param data
 * @param size
 *
 * @return 
 */
static  int burn_fw(const u8 *data, size_t size)
{
    int ret = 0;
    (void) (data);(void)(size);
    return ret;
}
//该代码一般放在probe或者手动通过sysfs触发
static int __init example_init(void)
{
    int ret = 0;
    struct device dev;
    const struct firmware *fw;
    const unsigned char *data = NULL;

    printk("==> %s\n", __func__);
    ret = request_firmware(&fw, "test.bin", &dev);
    if (ret < 0)
    {
        printk("failed to load file test.bin\n");
        goto out;
    }
    data = fw->data;
    printk("FW size [%u]\n", (unsigned int)fw->size);
    //add your code
    //for example
    /*
     * if (*(u32*) fw->data != 0x1234){ printk("invalid magic value!\n");}
     */
    ret = burn_fw(data, fw->size);
    if (ret < 0)
    {
        printk("burn fw failed\n");
        goto fw_release;
    }
    else{
        printk("burn successed\n");
    }
fw_release:
    release_firmware(fw);
out:
    printk("<== %s\n", __func__);
    return ret;
}
static void __exit example_exit(void)
{
    printk("==> %s\n", __func__);
    printk("<== %s\n", __func__);
}

module_init(example_init);
module_exit(example_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Silicon Labs C2 port Linux support for efm8sb1");
MODULE_AUTHOR("Joy.Hou <544088192@qq.com>");
