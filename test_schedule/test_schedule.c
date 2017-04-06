/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Jz.
 *       Filename:  test_schedule.c
 *
 *    Description:  
 *         Others: 1. schedule()函数会扔调度，如果该进程的状态认为RUNNING则不会变，
 *          如果进程状态被设置为可中断的则调度就真的主动放弃了。
 *
 *        Version:  1.0
 *        Created:  08/09/2016 11:05:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), houwentaoff@gmail.com
 *   Organization:  Jz
 *
 * =====================================================================================
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/sched.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("housir");

#define DEVNAME "housir_gpio"

static int major = 244;
static struct class *housir_class;
static struct cdev reg_read_cdev;

static int reg_open(struct inode *inode, struct file *filp);
static ssize_t reg_read(struct file *filp, char *buffer, size_t count, loff_t *ppos);
static ssize_t reg_write(struct file *file,const char __user *buf, size_t count, loff_t *ppos);

static struct file_operations reg_read_ops = {
      .owner   =  THIS_MODULE,
      .open    =  reg_open,
      .read    =  reg_read,
      .write   =  reg_write, 
};

static ssize_t reg_write(struct file *file,const char __user *buf, size_t count, loff_t *ppos)
{

    return count;
}

static int reg_open(struct inode *inode, struct file *filp)
{
    printk("===> %s\n", __func__);
    printk("<=== %s\n", __func__);

    return 0;
}

static ssize_t reg_read(struct file *filp, char *buffer, size_t count, loff_t *ppos)
{

    printk("===> %s\n", __func__);
    for (;;)//s
    {
    //    set_current_state(TASK_UNINTERRUPTIBLE);//此状态只能被wakeup唤醒，kill是唤不醒的

        set_current_state(TASK_INTERRUPTIBLE);//如果不加此句，进程的状态为RUNNING schedule看起来没有起作用
        if (signal_pending(current))
        {
            printk("signal pending!!\n");//收到信号
            break;
        }
        else
            schedule();// not schedule_work, 调用schedule后该进程就再也不会被调度除非wakeup或者给这进程发信号(kill -9...).
        // 被信号/wakeup 唤醒后会进行下面的打印, 并再去设置进程的状态和break
        printk("wake up by signal/wakeup \n");
    }
    set_current_state(TASK_RUNNING);
    printk("<=== %s\n", __func__);
    return 0;
}
static int reg_init(void)
{
    int result;
    float apple=1;
    dev_t devid;
    result = (int) apple;

    printk("===> %s\n", __func__);

    /* 初始化字符设备 */
    if (major)
    {
        devid = MKDEV(major,0);/* 主设备号是major，次设备号是0 */
        result = register_chrdev_region(devid, 32, DEVNAME);
    }
    else 
    {
        result = alloc_chrdev_region(&devid, 0, 32, DEVNAME);
        major = MAJOR(devid);
    }

#if 1
    if (result < 0) {
        printk("register-chrdev failed: %d\n", result);
        goto undo_request_region;
    }
    if (!major) {
        major = result;
        printk("got dynamic major %d\n", major);
    }
#endif

    cdev_init(&reg_read_cdev, &reg_read_ops);//初始化结构体struct cdev
//    reg_read_cdev.owner = THIS_MODULE;

    cdev_add (&reg_read_cdev, devid, 32);//将结构提struct cdev添加到系统之中

    housir_class = class_create(THIS_MODULE, "housir_class");
    if (IS_ERR(housir_class)) {
        printk(KERN_ERR "class_create() failed for housir_class\n");
        result = PTR_ERR(housir_class);
        goto out_chrdev;
    }

    device_create(housir_class, NULL, MKDEV(major, 0), NULL,
                  "housir_major%d", 0);    

    printk("<=== %s\n", __func__);
    return 0;

out_chrdev:
    printk("housir_init::register failure!!\n");
    unregister_chrdev_region(MKDEV(major, 0), 32);//卸载设备驱动所占有的资源
undo_request_region:
    return result;
}
static  void reg_exit(void)
{
    printk("===> %s\n", __func__);

    device_destroy(housir_class, MKDEV(major, 0));
    if (housir_class)
        class_destroy(housir_class);
    cdev_del(&reg_read_cdev);
    if (major)
        unregister_chrdev_region(MKDEV(major, 0), 32);//卸载设备驱动所占有的资源
   printk(KERN_INFO"demo exit\n");
   printk("<=== %s\n", __func__);
}
module_init(reg_init);
module_exit(reg_exit);

