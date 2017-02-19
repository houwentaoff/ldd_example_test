/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Jz.
 *       Filename:  test_tasklet.c
 *
 *    Description:  
 *         Others: 1. wake_up唤醒是更改当前进程的状态如D 不可中断的睡眠状态->就绪态
 *         的函数
 *                 2. 注意TASK_INTERRUPTIBLE和TASK_UNINTERRUPTIBLE 一个可以被kill唤醒一个不行
 *                 3. 完成量机制基于等待队列
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
#include <linux/workqueue.h>

#include <linux/interrupt.h>//tasklet
#include <linux/sched.h>//wait queue

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("housir");

#define DEVNAME "housir_gpio"

static int major = 0;
static struct class *housir_class;
static struct cdev reg_read_cdev;

//static struct tasklet_struct my_tasklet;
//wait_queue_head_t my_queue;
DECLARE_WAIT_QUEUE_HEAD(my_waitqueue);//declare and init :1.wait_queue_head_t test;+ init_waitqueue_head(&test);
bool ready = false;

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
//    tasklet_schedule(&my_tasklet);
//    ready = true;
    static bool test = true;
    test = !test;
    ready = test;
//    wake_up_interruptible(&my_waitqueue);//just TASK_INTERRUPTIBLE
    wake_up(&my_waitqueue);// TASK_ALL->TASK_INTERRUPTIBLE|TASK_UNINTERRUPTIBLE
    printk("queue schedule !!!\n");
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
    wait_event(my_waitqueue, ready == true);//TASK_UNINTERRUPTIBLE
//    wait_event_interruptible(my_waitqueue, ready == true);//TASK_INTERRUPTIBLE
    ready = false;//记得将条件改变否则这个进程状态就变不回来了

    printk("wait queue be wake up d.\n");
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
//    init_waitqueue_head(&my_queue);
//    printk("init tasklet!!!\n");
    /* 初始化字符设备 */
    if (major)
    {
        devid = MKDEV(major,0);/* 主设备号是major，次设备号是0 */
        result = register_chrdev_region(devid, 33, DEVNAME);
    }
    else 
    {
        result = alloc_chrdev_region(&devid, 0, 33, DEVNAME);
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

    cdev_add (&reg_read_cdev, devid, 33);//将结构提struct cdev添加到系统之中

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
    unregister_chrdev_region(MKDEV(major, 0), 33);//卸载设备驱动所占有的资源
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
        unregister_chrdev_region(MKDEV(major, 0), 33);//卸载设备驱动所占有的资源
//    tasklet_kill(&my_tasklet);
    printk(KERN_INFO"demo exit\n");
    printk("<=== %s\n", __func__);
}
module_init(reg_init);
module_exit(reg_exit);

