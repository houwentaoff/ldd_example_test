/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Jz.
 *       Filename:  test_mmap.c
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

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/kernel.h>
#include <linux/sched.h> /* include -> current*/
#include <linux/mman.h> /* PROT_READ MAP_SHARED... */
#include <linux/slab.h> /* kmalloc */

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("housir");

#define DEVNAME "housir_mmap"

static int major = 0;
static struct class *housir_class;
static struct cdev reg_read_cdev;
volatile unsigned long regs_addr = 0;
volatile unsigned long *virt_addr = 0;

static unsigned char *buffer = NULL;//

static int reg_open(struct inode *inode, struct file *filp);
static ssize_t reg_read(struct file *filp, char *buffer, size_t count, loff_t *ppos);
static ssize_t reg_write(struct file *file,const char __user *buf, size_t count, loff_t *ppos);
static int mmap(struct file *file, struct vm_area_struct *vma);

static struct file_operations reg_read_ops = {
      .owner   =  THIS_MODULE,
      .open    =  reg_open,
      .read    =  reg_read,
      .write   =  reg_write, 
      .mmap    =  mmap, 
};

static int mmap(struct file *file, struct vm_area_struct *vma)
{
//    unsigned long error;
//    unsigned long size = 100;
    int rval = 0;

//    down_write(&current->mm->mmap_sem);
//    error = do_mmap(file, 0, size, PROT_READ|PROT_WRITE, MAP_SHARED, 0);
//    up_write(&current->mm->mmap_sem);
    
    unsigned long page;

    page = virt_to_phys(buffer);
    printk("==>%s:start[0x0%x]size[%ld]\n", __func__,
            vma->vm_start, vma->vm_end-vma->vm_start);
    if (remap_pfn_range(vma, vma->vm_start, page>>PAGE_SHIFT, vma->vm_end-vma->vm_start, PAGE_SHARED) < 0)
    {
        rval = -1;
    }
    return rval;
}
static ssize_t reg_write(struct file *file,const char __user *buf, size_t count, loff_t *ppos)
{
#if 0    
    unsigned char bin_content_ascii[100] = {'\0'};
    unsigned int reg_addr = 0;
    ssize_t rc = 0;
    static char flag = 0;
    flag = ~flag;

    if(copy_from_user(bin_content_ascii,buf,count))
        return 0;
    printk(KERN_ALERT " write %s,%d\n",bin_content_ascii, count);
    sscanf(bin_content_ascii, "%x", &reg_addr);
    virt_addr = (unsigned long *)ioremap(reg_addr, sizeof(unsigned long));
    if (!virt_addr) {
        printk("Unable to map\n");
        rc = -ENOMEM;
        goto err_reg_map;
    }
    printk("phy addr[0x%x], value[0x%x]\n", reg_addr, *virt_addr);
    iounmap((void *)virt_addr);     
err_reg_map:
    return count;
#endif
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
                  "housir_mmap");    
    buffer = (unsigned char *)kmalloc(32*1024*1024, GFP_KERNEL);//imx6 测试为 最大32M 
    if (!buffer)
    {
        goto clear_all;
    }
    buffer[0] = 0x0;
    buffer[1] = 0x1;
    buffer[2] = 0x2;
    buffer[3] = 0x3;
    printk("<=== %s\n", __func__);
    return 0;
clear_all:
    device_destroy(housir_class, MKDEV(major, 0));
    if (housir_class)
        class_destroy(housir_class);
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
    if (!buffer)
        kfree(buffer);
   printk(KERN_INFO"demo exit\n");
   printk("<=== %s\n", __func__);
}
module_init(reg_init);
module_exit(reg_exit);

