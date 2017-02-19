/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Jz.
 *       Filename:  test_tasklet_hi.c
 *
 *    Description:  
 *         Others: 1. 尽可能的快的执行小任务，执行的开始时间不定，在任务执行的函数中不允许使用带阻塞
 *         的函数
 *                 3. 下半部运行时是允许中断请求的，而上半部运行时是关中断的, tasklet会被
 *                 中断打断。
 *                 4. 这个在声卡和网卡中常使用，在CPU的tasklet_hi_vec中排队
 *                 5. 使用HI_SOFTIRQ作为软中断,而不是TASKLET_SOFTIRQ
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

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("housir");

#define DEVNAME "housir_gpio"

static int major = 0;
static struct class *housir_class;
static struct cdev reg_read_cdev;
volatile unsigned long regs_addr = 0;
volatile unsigned long *virt_addr = 0;

static struct tasklet_struct tasklet_capture;

static int reg_open(struct inode *inode, struct file *filp);
static ssize_t reg_read(struct file *filp, char *buffer, size_t count, loff_t *ppos);
static ssize_t reg_write(struct file *file,const char __user *buf, size_t count, loff_t *ppos);

static struct file_operations reg_read_ops = {
      .owner   =  THIS_MODULE,
      .open    =  reg_open,
      .read    =  reg_read,
      .write   =  reg_write, 
};
static void tasklet_handler (unsigned long data)
{
    struct timeval time;
    do_gettimeofday(&time);
    printk("hi tasklet_handler is running.s:%ld us:%ld\n", time.tv_sec, time.tv_usec);
}
static ssize_t reg_write(struct file *file,const char __user *buf, size_t count, loff_t *ppos)
{
    struct timeval time;

    tasklet_hi_schedule(&tasklet_capture);
    do_gettimeofday(&time);
    printk("hi tasklet schedule !!! s:%ld us:%ld \n", time.tv_sec, time.tv_usec);
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
    tasklet_init(&tasklet_capture,tasklet_handler,0);
    printk("init tasklet!!!\n");
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
    tasklet_kill(&tasklet_capture);
    printk(KERN_INFO"demo exit\n");
    printk("<=== %s\n", __func__);
}
module_init(reg_init);
module_exit(reg_exit);

