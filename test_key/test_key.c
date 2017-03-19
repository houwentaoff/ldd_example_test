/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  test_key.c
 *
 *    Description: 中断线程化,ps查看新加的一个中断一个线程，进程上下文可sleep.
 *         Others: 尝试中断绑定cpu失败
 *                 查看/proc/interrupt 每个cpu触发的中断和是否和其他中断重复.
 *
 *        Version:  1.0
 *        Created:  Wednesday, July 20, 2016 10:27:02 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy. Hou (hwt), 544088192@qq.com
 *   Organization:  xxx
 *
 * =====================================================================================
 */
#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#include <asm/irq.h>
#include <asm/io.h>
#include <asm/current.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joy Hou");

static irqreturn_t top_interrupt(int irq, void *dummy)
{
    printk("top!!!!\n");
	return IRQ_WAKE_THREAD;/* 返回此标记才会唤醒botton */
}
static irqreturn_t botton_interrupt(int irq, void *dummy)
{
    int value = 0;
//	input_report_key(button_dev, BTN_0, inb(BUTTON_PORT) & 1);
//	input_sync(button_dev); 
	//current_thread_info()->cpu = 2;
//    struct cpumask new_affinity;

//    cpumask_copy(&new_affinity, cpu_online_mask);
//    new_affinity.bits[0] = 0x2;
//    irq_set_affinity(irq, &new_affinity);
//    printk("0x%X\n", new_affinity);
    
//    local_bh_disable();
//    local_irq_disable();
//    int irq = 0;
//    irq = gpio_to_irq(IMX_GPIO_NR(1, 28));
//    disable_irq_nosync(irq);
    printk("irq action begin!!!irq[%d]cpu[%u]\n", irq, current_thread_info()->cpu);

    //中断线程可以中可以使用msleep  
    //msleep(3000);
    mdelay(3000);
    value = gpio_get_value(IMX_GPIO_NR(1,28));
    if (value == 0/* LOW */)
    {
        printk("press > 3s\n");
    }
//   
    printk("irq action end!!!cpu[%u]gpio value[%d]\n", current_thread_info()->cpu, value);
//    enable_irq(irq);
//    local_irq_enable();
//    local_bh_enable();
//    printk("irq action end!!!cpu[%u]\n", current->cpu);
	return IRQ_HANDLED;
}
static int __init button_init(void)
{
	int error;
    int irq = 0;

    cpumask_var_t mask;
    cpumask_set_cpu(2, mask);
    irq = gpio_to_irq(IMX_GPIO_NR(1, 28));
#if 1
    if ((error = request_threaded_irq(irq, top_interrupt, botton_interrupt, IRQF_TRIGGER_FALLING, "button", NULL)) != 0)
    {
        printk("err[%d]\n", error);
        return -EBUSY;
    }
    irq = gpio_to_irq(IMX_GPIO_NR(4, 5));
    if ((error = request_threaded_irq(irq, NULL, botton_interrupt, IRQF_TRIGGER_FALLING, "button", NULL)) != 0)
    {
        printk("err[%d]\n", error);
        return -EBUSY;
    }
//    irq_set_affinity(irq, &mask);//不起作用
#else
	if (error = request_irq(irq, button_interrupt, IRQF_NOBALANCING|IRQF_TRIGGER_FALLING, "button", NULL)) {
        printk(KERN_ERR "button.c: Can't allocate irq %d err %d\n", irq, error);
        return -EBUSY;
    }
//    irq_set_affinity(irq, &mask);//不起作用
#endif
	return 0;
}

static void __exit button_exit(void)
{
    int irq = 0;
    irq = gpio_to_irq(IMX_GPIO_NR(1, 28));
	free_irq(irq, NULL);
    irq = gpio_to_irq(IMX_GPIO_NR(4, 5));
	free_irq(irq, NULL);

}

module_init(button_init);
module_exit(button_exit);

