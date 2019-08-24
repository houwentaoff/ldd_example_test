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
 *                  5. 注意api中的device参数
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
#include <linux/slab.h>
#include <linux/workqueue.h> 
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

//static struct work_struct work;   
static void work_func(struct work_struct *work)
{    
    int ret = 0;
//    struct device dev;
    const struct firmware *fw;
    const unsigned char *data = NULL;

    printk("work queue wake  exec!!!\n");
    ret = request_firmware(&fw, "test.bin", NULL);//注意device参数
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
    return ;
}

//该代码一般放在probe或者手动通过sysfs触发
static int __init example_init(void)
{
    int ret = 0;
//    INIT_WORK(&work, work_func); 
//    schedule_work(&work); 
    // 在内核4.14 使用该API会配合udev进行操作会wait，若udev和对应的sys/class未配置好，会一直wait : 因为device参数传成了没人使用的device导致死锁，卡住
    // 可暂时使用如下代码代替
    work_func(NULL);
#ifdef DONT_USE_FM
    enum kernel_read_file_id id = READING_FIRMWARE; 
    char *path = NULL;
    size_t msize = INT_MAX;
    loff_t size;
    int  len;

#endif
    printk("==> %s\n", __func__);
#ifdef DONT_USE_FM
    path = __getname();

    len = snprintf(path, 128, "%s/%s",
            "/lib/firmware", "test.bin");
    printk("len[%d] path[%s]\n", len, path);
    ret = kernel_read_file_from_path(path, (void **)&data, &size, msize,
            id);

    printk("%d\n", ret);
    if (ret == 0)
    {
        printk("size[%d], msize[%d]data[%s]\n", size, msize, data);
    }
    __putname(path);
#endif

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
