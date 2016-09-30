/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, xxx.
 *       Filename:  test_input_key.c
 *
 *    Description:  
 *         Others:
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

#include <asm/irq.h>
#include <asm/io.h>

extern void msleep(unsigned int mic);
static struct input_dev *button_dev;

//static irqreturn_t button_interrupt(int irq, void *dummy)
//{
//	input_report_key(button_dev, BTN_0, inb(BUTTON_PORT) & 1);
//	input_sync(button_dev);
//	return IRQ_HANDLED;
//}
static int button_open(struct input_dev *dev) 
{
    return 0;
}
static void button_close(struct input_dev *dev)
{
    return ;
}
/**
 * @brief close的时候被触发
 *              evdev_write->input_inject_event
 *
 * @param dev
 * @param file
 *
 * @return 
 */
static int button_flush(struct input_dev *dev, struct file *file)
{
    printk("[event]:==>%s\n", __func__);
    printk("[event]:<==%s\n", __func__);
    return 0;
}
/**
 * @brief EV_SYN + SYN_CONFIG 被触发
 *
 * @param dev
 * @param type
 * @param code
 * @param value
 *
 * @return 
 */
static int button_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)
{
    printk("[event]:==>%s type[0x%x] code[0x%x] value[0x%x]\n",
            __func__, type, code, value);
    if (EV_SYN == type && SYN_CONFIG == code)
    {
        ;
//        KeyCode = value&0xff;
//        kill_fasync(&key_async, SIGIO, POLL_IN);
    }
    printk("[event]:<==%s\n", __func__);
    return 0;
}
static int __init button_init(void)
{
	int error;

//	if (request_irq(BUTTON_IRQ, button_interrupt, 0, "button", NULL)) {
//                printk(KERN_ERR "button.c: Can't allocate irq %d\n", button_irq);
//                return -EBUSY;
//        }

	button_dev = input_allocate_device();
	if (!button_dev) {
		printk(KERN_ERR "button.c: Not enough memory\n");
		error = -ENOMEM;
		goto err_free_irq;
	}

	button_dev->evbit[0] = BIT_MASK(EV_KEY);
	button_dev->keybit[BIT_WORD(BTN_0)] = BIT_MASK(BTN_0);
    button_dev->name = "buttons_tyc";
    button_dev->phys = "/dev/input00";//没啥效果，只是作为显示作用？
    
    button_dev->dev.init_name = "input_tyc";  
    button_dev->open = button_open; 
    button_dev->close = button_close; 
    button_dev->flush = button_flush;
    /* led 和 sound 目前不被触发 why???  设置了evbit的仍然不被触发 */
    button_dev->event = button_event;//led or sound :operation to device   (EV_SYN+SYN_CONFIG) only?

	error = input_register_device(button_dev);
	if (error) {
		printk(KERN_ERR "button.c: Failed to register device\n");
		goto err_free_dev;
	}
	input_report_key(button_dev, BTN_0, 1);
	input_sync(button_dev);
    msleep(5000);
	input_report_key(button_dev, BTN_0, 1);
	input_sync(button_dev);
    msleep(5000);
	input_report_key(button_dev, BTN_0, 1);
	input_sync(button_dev);
    msleep(5000);
	input_report_key(button_dev, BTN_0, 0);
	input_sync(button_dev);
    msleep(5000);
	input_report_key(button_dev, BTN_0, 1);
	input_sync(button_dev);
	return 0;

 err_free_dev:
	input_free_device(button_dev);
 err_free_irq:
//	free_irq(BUTTON_IRQ, button_interrupt);
	return error;
}

static void __exit button_exit(void)
{
    input_unregister_device(button_dev);
//	free_irq(BUTTON_IRQ, button_interrupt);
}

module_init(button_init);
module_exit(button_exit);

