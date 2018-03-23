/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  test_misc.c
 *
 *    Description:  测试混杂设备
 *                  1. 设置次设备号前需要查看`cat /proc/misc`
 *                  已存在的次设备号避免冲突.
 *                  2. 也可利用动态的次设备号
 *         Others:
 *                  1. 实例：zebra 斑马扫描头:在原生的OV5640 sensor驱动上加入了瞄准灯和补光灯的附加操作
 *                  对应的混杂设备文件为/dev/sdl_control id为动态的 MISC_DYNAMIC_MINOR
 *                  2. 原生的sensor驱动位于media/video/mxc/capture/0v5640.c 是非字符设备的驱动，而是无设备
 *                  文件映射出来的驱动(支持V4L2),zebra的手册提供了额外的i2c命令（通过misc设备的ioctl操作瞄准灯和补光灯的
 *                  相关操作);
 *                  看起来混杂设备为附加到其它设备上的驱动。用于给APP可操作附加设备的接口而做的(因为misc中有file_operation结构).
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
    //unlocked_ioctl,
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
