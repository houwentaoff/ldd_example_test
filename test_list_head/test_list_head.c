/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  test_platform1.c
 *
 *    Description:  定时器 精度大约ms级别 走软中断
 *                  
 *         Others:  
 *
 *        Version:  1.0
 *        Created:  Sunday, August 14, 2016 10:48:35 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */

#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/slab.h> 
#include <asm/uaccess.h> 
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joy.Hou");

#define DEVNAME "housir_gpio"

static struct timer_list my_timer;
static struct class *timer_class;
static struct device *test_dev1;
unsigned int timeperiod = 0;
//static struct device *test_dev2;

struct priv_data{
    struct list_head list_head;
    atomic_t has_data;
};
struct global_data{
    /* 连接上priv_data */
    struct list_head list_head;
    wait_queue_head_t wait;
    unsigned long data;
    atomic_t ref_cnt;
};
static struct global_data gld;
static int major = 0;
static struct class *housir_class;                                                                                                                                  
static struct cdev reg_read_cdev;

static int reg_open(struct inode *inode, struct file *filp);
static ssize_t reg_read(struct file *filp, char *buffer, size_t count, loff_t *ppos);
static int release(struct inode *in, struct file *fp);

static struct file_operations reg_read_ops = {
    .owner   =  THIS_MODULE,
    .open    =  reg_open,
    .read    =  reg_read,
    .release = release,
};
static ssize_t reg_read(struct file *file, char *buffer, size_t count, loff_t *off)
{       
    struct priv_data *pri = NULL;
    ssize_t retval=0;
    DECLARE_WAITQUEUE(wait, current);
    unsigned long d = 0;
    char str[10] = {0};
    int len = 0;

//    if (*off > 0)
//        return 0;
    printk("===> %s:%p\n", __func__, file);  
    pri = file->private_data;
    add_wait_queue(&gld.wait, &wait);
    for ( ; ; ) {
        set_current_state(TASK_INTERRUPTIBLE);
        if (atomic_read(&pri->has_data) == 1){
            atomic_set(&pri->has_data, 0);
            break;
        }else if (file->f_flags & O_NONBLOCK) {
            retval = -EAGAIN;
            goto out;
        } else if (signal_pending(current)) {
            retval = -ERESTARTSYS;
            goto out;
        }      
        schedule();
    }          
#if 0
    retval = put_user(gld.data, (unsigned long __user *)buffer);
    if (!retval)
        retval = sizeof(unsigned long);
#else 
    d = gld.data;
    sprintf(str, "%lu\n", d);
    len = strlen(str) + 1;
    if (copy_to_user(buffer, str, len))
    {
        retval = -EFAULT;
    }
    *off += len;
    printk("data[%lu]\n", d);
#endif
out:           
    __set_current_state(TASK_RUNNING);
    remove_wait_queue(&gld.wait, &wait);

    printk("<=== %s:%p\n", __func__, file);
    return len;
}       

static int reg_open(struct inode *inode, struct file *filp)
{                    
    struct priv_data * pri = NULL;                                                                                                                                   
    printk("===> %s\n", __func__);

    pri = kzalloc(sizeof (struct priv_data), GFP_KERNEL);
    
    if (atomic_read(&gld.ref_cnt) == 0)
    {
        printk("we has been init!\n");

        /*-----------------------------------------------------------------------------
         *  do other...
         *-----------------------------------------------------------------------------*/
    }
    
    atomic_inc(&gld.ref_cnt);
    list_add(&pri->list_head, &gld.list_head);
    filp->private_data = pri;   
    printk("<=== %s\n", __func__);

    return 0;        
}                
static int release(struct inode *in, struct file *fp)
{
    struct priv_data * pri = fp->private_data; 
    printk("===> %s\n", __func__);   
    atomic_dec(&gld.ref_cnt);
    if (atomic_read(&gld.ref_cnt) == 0)
    {
        ;
    }
    else 
    {
        ;
    }
    if (pri)
    {
        list_del(&pri->list_head);
        kfree(pri);
    }

    printk("<=== %s\n", __func__);      
    return 0;
}
/*
 * The "foo" file where a static variable is read from and written to.
 */
static ssize_t on_show(struct kobject *kobj, struct kobj_attribute *attr,
    char *buf)
{
      return sprintf(buf, "%s\n", timer_pending(&my_timer) ? "true" : "false");
}

static ssize_t on_store(struct kobject *kobj, struct kobj_attribute *attr,
      const char *buf, size_t count)
{
    unsigned int flag = 0;
    sscanf(buf, "%u", &flag);
    switch (flag)
    {
        case 1:
            if (timeperiod != 0)
            {
                mod_timer(&my_timer, jiffies + timeperiod);
            }
            break;
        case 0:
            //删除时记得查询是否还在全局链中，如不在链中会出错
            //也要防止正在被触发的的时候被删除。
            if (timer_pending(&my_timer))
            {
                del_timer_sync(&my_timer);
            }
            break;
        default:
            break;
    }
    return count;
}
static ssize_t value_store(struct kobject *kobj, struct kobj_attribute *attr,
      const char *buf, size_t count)
{
    unsigned int mstime = 0;
    unsigned int stime = 0;
    sscanf(buf, "%u", &stime);
//    printk("time = %us.\n", stime);
    mstime = stime *HZ/1000;//s HZ-> ms 
    timeperiod = mstime;
    printk("time period = %ums.\n", stime);
    mod_timer(&my_timer, jiffies + mstime);
    printk("finish.\n");
    return count;
}

//static struct device_attribute foo_attribute =
//      __ATTR(value, 0666, foo_show, foo_store);
static DEVICE_ATTR(value, S_IWUSR, NULL, value_store);

static DEVICE_ATTR(on, S_IWUSR|S_IRUSR , on_show, on_store);

/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *attrs[] = {
      &dev_attr_value.attr,
      &dev_attr_on.attr,
      NULL, /* need to NULL terminate the list of attributes */
};

/*
 * An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.
 */
static struct attribute_group attr_group = {
      .attrs = attrs,
};
static void timer_func(struct timer_list * t)
{
    struct priv_data *pri = NULL;  
    
    gld.data++;

    list_for_each_entry(pri, &gld.list_head, list_head){
        atomic_set(&pri->has_data, 1);
    }
//    struct list_head * cur;
//    list_for_each(cur, gld.list_head) {
//        transport = list_entry(elae, struct transport, list);
//
//    }
    
    wake_up_interruptible(&gld.wait);
    my_timer.expires = jiffies + timeperiod;
    add_timer(&my_timer);
}
#if 0
static void start_timer(void)
{
    timer_setup(&my_timer, timer_func, 0);
//    my_timer.expires = jiffies + 1*HZ/100;
//    add_timer(&my_timer);
}
#endif
static int __init example_init(void)
{
    int retval = 0;
    dev_t devid;   
    int result;

    timer_class   = class_create(THIS_MODULE, "timer");
    test_dev1   = device_create(timer_class, NULL, 0, NULL, "timer1");//timer1/目录下创建xxx目录
    retval = sysfs_create_group(&test_dev1->kobj, &attr_group);
    if (retval)
    {
        printk("[test sysfs]: sysfs create group fail.\n");
    }
    result = alloc_chrdev_region(&devid, 0, 33, DEVNAME);
    major = MAJOR(devid);     
    if (result < 0) {
        printk("register-chrdev failed: %d\n", result);
        goto undo_request_region;
    }       
    if (!major) {                                                                                                                                                   
        major = result;
        printk("got dynamic major %d\n", major);
    }       
    cdev_init(&reg_read_cdev, &reg_read_ops);
    cdev_add (&reg_read_cdev, devid, 33);
    housir_class = class_create(THIS_MODULE, "housir_class");
    if (IS_ERR(housir_class)) {
        printk(KERN_ERR "class_create() failed for housir_class\n");
        result = PTR_ERR(housir_class);
        goto out_chrdev;                                                                                                                                            
    }                   

    device_create(housir_class, NULL, MKDEV(major, 0), NULL,
            "housir_major%d", 0);
    timer_setup(&my_timer, timer_func, 0);
    //需要使用add或者Mod加入内核的全局定时器链 

    INIT_LIST_HEAD(&gld.list_head);
    init_waitqueue_head(&gld.wait);

    return retval;
out_chrdev:
    printk("housir_init::register failure!!\n");                                                                                                                    
    unregister_chrdev_region(MKDEV(major, 0), 33);//卸载设备驱动所占有的资源
undo_request_region:
    return result;    
}

static void __exit example_exit(void)
{
    printk("==> %s\n", __func__);
    if (timer_pending(&my_timer))
    {
        del_timer_sync(&my_timer);
    }
    device_del(test_dev1);
    if (timer_class)
    {
        class_destroy(timer_class);
    }
    device_destroy(housir_class, MKDEV(major, 0));
    if (housir_class)        
        class_destroy(housir_class);
    cdev_del(&reg_read_cdev);                                                                                                                                       
    if (major)               
        unregister_chrdev_region(MKDEV(major, 0), 33);//卸载设备驱动所占有的资源

    printk("<== %s\n", __func__);
    printk("demo exit\n");
}

module_init(example_init);
module_exit(example_exit);
