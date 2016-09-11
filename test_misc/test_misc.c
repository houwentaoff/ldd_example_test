/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  test_misc.c
 *
 *    Description:  测试混杂设备
 *                  1. 设置次设备号前需要查看`cat /proc/misc`
 *                  已存在的次设备号避免冲突.
 *         Others:
 *
 *        Version:  1.0
 *        Created:  Sunday, September 11, 2016 07:03:47 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

unsigned int param=0;

static int my_open(struct inode *node,struct file *filep )
{
    printk("Open my_open sucess!\n");
    return 0;
}
static int my_read(struct file *filp,char __user *buf,size_t size,loff_t *off)
{
    int len = 0;
    char buf1[]={"this is buf!\n"};

    if (*off > 0)
        return 0;//必须做处理,否则会一直读
    len = strlen(buf1)+1;
    printk("Read my_read sucess!\n");
    param = 500;
//    copy_to_user(buf, &param, 4);
    copy_to_user(buf, buf1, sizeof(buf1));
    *off += len;
    return len;
}


struct file_operations my_miscdev_fops =
{    
    .open = my_open,
    .read = my_read,
};

struct miscdevice my_miscdev = 
{
    .minor = 201,
    .name =  "my_miscdev",// 设备节点:/dev/my_miscdev
    .fops = &my_miscdev_fops,
};
static int my_miscdev_init(void)
{
    int ret;
    ret = misc_register(&my_miscdev);
    if(ret != 0)
        printk("miscdev register fail.\n!");
    return 0;

}
static void my_miscdev_exit(void)
{
    misc_deregister(&my_miscdev);
}

MODULE_LICENSE("GPL");

module_init(my_miscdev_init);
module_exit(my_miscdev_exit);
