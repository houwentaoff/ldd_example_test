/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Sobey.
 *       Filename:  proc_test01.c
 *
 *    Description:  3.10 proc_create接口发生改变
 *         Others:
 *
 *        Version:  1.0
 *        Created:  Tuesday, March 08, 2016 11:00:12 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sean. Hou (hwt), houwentaoff@gmail.com
 *   Organization:  Sobey
 *
 * =====================================================================================
 */
#include <linux/module.h>
#include <linux/sched.h> //jiffies
#include <linux/uaccess.h> //copy_to|from_user()
#include <linux/slab.h>
#include <linux/proc_fs.h>

static char *str = NULL;


static ssize_t my_proc_read (struct file *filp, char __user *buf, size_t size, loff_t *offset);
static ssize_t my_proc_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset);
static const struct file_operations test_proc_fops = {
    .owner = THIS_MODULE,
    .read  = my_proc_read,
    .write = my_proc_write,
};
//proc文件的读函数
/**
 * @brief 
 *
 * @param filp
 * @param buf
 * @param size
 * @param off 每次读完后off的偏移增加当前读的大小
 *
 * @return 0:读完了 total:此次读的大小，若只返回total则会导致一直输出....
 */
static ssize_t my_proc_read (struct file *filp, char __user *buf, size_t size, loff_t *off)
{
    int len = 0;
    int total = 0;
    
    if (*off > 0)
        return 0;
	len = sprintf(buf, "current kernel time is %ld\n", jiffies);
	len += sprintf(buf+len, "str is %s\n", str);
    total += len;
   
    *off += total;

	return total;
}

//proc文件的写函数
#ifdef LEGECY_CODE
static int my_proc_write(struct file *filp, const char __user *buf, unsigned long count, void *data)
#else
static ssize_t my_proc_write (struct file *filp, const char __user *buf, size_t size, loff_t *off)
{
	//分配临时缓冲区
	char *tmp;
    tmp = kzalloc((size+1), GFP_KERNEL);
	if (!tmp)
		return -ENOMEM;

	//将用户态write的字符串拷贝到内核空间
	//copy_to|from_user(to,from,cnt)
	if (copy_from_user(tmp, buf, size)) {
		kfree(tmp);
		return -EFAULT;
	}

	//将str的旧空间释放，然后将tmp赋值给str
	kfree(str);
	str = tmp;

	return size;
}
#endif

static int __init my_init(void)
{
	struct proc_dir_entry *file;

	//创建proc文件
	file = proc_create("jif", 0666, NULL, &test_proc_fops);// legecy -> create_proc_entry("jif", 0666, NULL);
	if (!file) {
		printk("Cannot create /proc/jif\n");
		return -1;
	}

#ifdef LEGECY_CODE
	//将创建好的文件和读写函数关联在一起
//	file->read_proc = my_proc_read;
//	file->write_proc = my_proc_write;
#endif

	return 0;
}

static void __exit my_exit(void)
{
	//删除proc文件
	remove_proc_entry("jif", NULL);
	kfree(str);
}

module_init(my_init);
module_exit(my_exit);
MODULE_AUTHOR("aran");
MODULE_LICENSE("GPL");

