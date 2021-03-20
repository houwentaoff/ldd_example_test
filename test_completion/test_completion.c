/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Jz.
 *       Filename:  echo_reg.c
 *
 *    Description:  
 *         Others: 
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

/*-----------------------------------------------------------------------------
 *  1. 如何实现信号的保存
 *
 *  ```
static inline long __sched
do_wait_for_common(struct completion *x,
		   long (*action)(long), long timeout, int state)
{
	if (!x->done) {//如果done变量存在 则直接被唤醒
		DECLARE_WAITQUEUE(wait, current);

		__add_wait_queue_tail_exclusive(&x->wait, &wait);
		do {
			if (signal_pending_state(state, current)) {
				timeout = -ERESTARTSYS;
				break;
			}
			__set_current_state(state);
			spin_unlock_irq(&x->wait.lock);
			timeout = action(timeout);
			spin_lock_irq(&x->wait.lock);
		} while (!x->done && timeout);
		__remove_wait_queue(&x->wait, &wait);
		if (!x->done)
			return timeout;
	}
	x->done--;// 靠变量done++ done--保存已经发送的信号量
	return timeout ?: 1;
}

 *  ```
 *-----------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/uaccess.h> 

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("housir");

#define DEVNAME "housir_gpio"

static int major = 227;
static struct class *housir_class;
static struct cdev reg_read_cdev;
volatile unsigned long regs_addr = 0;
volatile unsigned long *virt_addr = 0;

static spinlock_t lock;
struct completion done;
static char str[100]={0};

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
    if (count < 100)
    {
        copy_from_user(str, buf, count);
    }
    else
    {
        strcpy(str, "over flow...");
    }
    complete(&done);
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

    /*-----------------------------------------------------------------------------
     *  D:TASK_UNINTERRUPTIBLE 不可中断的睡眠状态,　可处理signal,　有延迟
     *  进程不响应异步信号 kill -9 杀不死
     *  存在的意义就在于，内核的某些处理流程是不能被打断的。如果响应异步信号，程序
     *  的执行流程中就会被插入一段用于处理异步信号的流程（这个插入的流程可能只存在
     *  于内核态，也可能延伸到用户态），于是原有的流程就被中断了。
     *                                            （参见《linux内核异步中断浅析》）
     *
     *  在进程对某些硬件进行操作时（比如进程调用read系统调用对某个设备文件进行读操
     *  作，而read系统调用最终执行到对应设备驱动的代码，并与对应的物理设备进行交互）
     *  ，可能需要使用TASK_UNINTERRUPTIBLE状态对进程进行保护，以避免进程与设备交互
     *  的过程被打断，造成设备陷入不可控的状态。这种情况下的TASK_UNINTERRUPTIBLE状
     *  态总是非常短暂的，通过ps命令基本上不可能捕捉到。 linux系统中也存在容易捕捉
     *  的TASK_UNINTERRUPTIBLE状态。执行vfork系统调用后，父进程将进入TASK_UNINTER-
     *  RUPTIBLE状态，直到子进程调用exit或exec（参见《神奇的vfork》）。
     *
     *  1. 发3个信号将会触发3次wait_for_completion,如果没有wait_for..则会保存于内核
     *-----------------------------------------------------------------------------*/
    wait_for_completion(&done);//状态为D 
    /* 可使用 wait_for_completion_interruptible 表明可被signal 信号唤醒,
     * 否则只有echo x>/dev/xx 唤醒该任务  
     */
    printk("str[%s]\n", str);
    printk("<=== %s\n", __func__);
    return 0;
}

static int reg_init(void)
{
    int result ;
    float apple=1;
    dev_t devid;
    result = (int) apple;

    printk("===> %s\n", __func__);
    spin_lock_init(&lock);
    init_completion(&done);

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
//        dev_err(&pdev->dev, "register-chrdev failed: %d\n", result);
        printk("register-chrdev failed: %d\n", result);
        goto undo_request_region;
    }
    if (!major) {
        major = result;
//        dev_dbg(&pdev->dev, "got dynamic major %d\n", major);
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
//    release_region(pc8736x_gpio_base, PC8736X_GPIO_RANGE);

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

