/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Jz.
 *       Filename:  echo_reg.c
 *
 *    Description:  添加了file中的privatedata. private数据是自定义的结构体.
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
#include <linux/slab.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("housir");

#define DEVNAME "housir_gpio"

static int major = 244;
static struct class *housir_class;
static struct cdev reg_read_cdev;
volatile unsigned long regs_addr = 0;
volatile unsigned long *virt_addr = 0;

struct testdata{
    int a;
    int b;
    int c;
};
#define TEST_IOC_MAGIC          'a'

#define TEST_IOC_WRITE          _IO(TEST_IOC_MAGIC, 10)
#define TEST_IOC_READ           _IO(TEST_IOC_MAGIC, 11)


static int reg_open(struct inode *inode, struct file *filp);
static ssize_t reg_read(struct file *filp, char *buffer, size_t count, loff_t *ppos);
static ssize_t reg_write(struct file *file,const char __user *buf, size_t count, loff_t *ppos);
static long reg_ioctl(
	       struct file *filp, unsigned int cmd, unsigned long arg);
static int release(struct inode *in, struct file *fp);
static struct file_operations reg_read_ops = {
      .owner   =  THIS_MODULE,
      .open    =  reg_open,
      .read    =  reg_read,
      .write   =  reg_write, 
      .unlocked_ioctl = reg_ioctl,
      .release = release,
};
static long reg_ioctl(
         struct file *filp, unsigned int cmd, unsigned long arg)
{
    int retval = 0;
    u32 tmp=0;
    
    switch (cmd){
    case TEST_IOC_WRITE:
        retval = get_user(tmp, (__u8 __user *)arg);
        printk("user:[0x%x]\n", tmp);
        break;
    case TEST_IOC_READ:
        retval = put_user(0x1, (__u8 __user *)arg);
        break;
    default:
        break;
    }
    return retval;
}

static int release(struct inode *in, struct file *fp)
{
    printk("===> %s\n", __func__);
    if (fp->private_data)
    {
        printk("pri is not null,free it!\n");
        kfree(fp->private_data);
    }
    else
    {
        printk("pri is null\n");
    }
    printk("<=== %s\n", __func__);
}
static ssize_t reg_write(struct file *file,const char __user *buf, size_t count, loff_t *ppos)
{
    unsigned char bin_content_ascii[100] = {'\0'};
    unsigned int reg_addr = 0;
    ssize_t rc = 0;
    static char flag = 0;
    struct testdata *pri = NULL;

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
	/*
	   ioremap 可使用devm_ioremap进行替换，如下:
		  static int cdns_spi_probe(struct platform_device *pdev)
		  一般该操作在probe中完成，其它函数中使用regs，参数传递&platform_device->dev 即可
	   void __iomem *crl_apb_rst = devm_ioremap(&pdev->dev, 0x1122334455, sizeof(unsigned int));
		  devm_ioremap 优点是devm系列函数不用调用iounmap
	*/
    printk("phy addr[0x%x], value[0x%x]\n", reg_addr, *virt_addr);
    iounmap((void *)virt_addr);     

    pri = file->private_data;
    pri->a = ++pri->a;
    pri->b = pri->b++;

err_reg_map:
    return count;
}

static int reg_open(struct inode *inode, struct file *filp)
{
    printk("===> %s\n", __func__);
    struct testdata * pri = NULL;
    
    pri = kzalloc(sizeof (struct testdata), GFP_KERNEL);
    
    filp->private_data = pri;

    printk("<=== %s\n", __func__);

    return 0;
}

static ssize_t reg_read(struct file *filp, char *buffer, size_t count, loff_t *ppos)
{
    struct testdata *pri = NULL;
    printk("===> %s\n", __func__);
    pri = filp->private_data;

    printk("<=== %s a[%d]b[%d]c[%d]\n", __func__, pri->a, pri->b, pri->c);
    return 0;
}
struct test_data{
    struct device *dev;
    spinlock_t lock;
};
struct test_data xxx={0};
static int foo = 0;

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
    char *buf)
{
      return sprintf(buf, "%d\n", foo);
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
      const char *buf, size_t count)
{
     sscanf(buf, "%du", &foo);
     printk("foo = %d.\n",foo);

     foo = foo -1;

     if  (foo < 0 || foo > 1)
     {
           printk("input foo error. foo=%d.\n",foo);
           return 0;
     }
      
     printk("finish.\n");
     return count;
}

static DEVICE_ATTR(value, S_IWUSR | S_IRUGO, foo_show, foo_store);

/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *attrs[] = {
      &dev_attr_value.attr,
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

static int reg_init(void)
{
    int result,err ;
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

    xxx.dev = device_create(housir_class, NULL, MKDEV(major, 0), NULL,
                  "housir_major%d", 0);    
    dev_set_drvdata(xxx.dev, xxx);
    result = sysfs_create_group(&xxx.dev->kobj, &attr_group);

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

